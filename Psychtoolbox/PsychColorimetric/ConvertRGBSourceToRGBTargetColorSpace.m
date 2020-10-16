function [MCSC, image] = ConvertRGBSourceToRGBTargetColorSpace(srcGamut, dstGamut, image)
% [MCSC, outImage] = ConvertRGBSourceToRGBTargetColorSpace(srcGamut, dstGamut [, image]);
%
% Convert an image from a RGB source colorspace to a RGB target colorspace.
%
% Given two 2-by-4 matrices with the CIE 1931 2D chromaticity coordinates of
% the color primaries red, green, blue and white-point of a source RGB
% colorspace and a destination RGB colorspace, build and return a 3-by-3
% colorspace conversion matrix to convert colors from the source colorspace
% to the target colorspace. Optionally apply the matrix to a RGB or RGBA
% input image, converting it from source to target colorspace.
%
% 'srcGamut' must be the 2-by-4 matrix with the primaries and white-point
% of the source color gamut / colorspace.
%
% 'dstGamut' can be a 2-by-4 matrix with the primaries and white-point
% of the destination color gamut / colorspace. Alternatively, you can pass
% in the window handle of an open Psychtoolbox onscreen window. In the
% latter case, the function will query that onscreen window for its
% currently assigned colorspace as a convenience. Please note that
% onscreen windows do not have any specific colorspace assigned by
% default, so passing in a window handle to such a window will result in
% this function turning into a no-operation, ie. it will return an identity
% 3-by-3 matrix and do nothing to a passed in image. HDR onscreen windows
% as requested via PsychImaging('AddTask', 'General', 'EnableHDR'); will
% have a suitable HDR display colorspace assigned, e.g., the Rec-2020
% colorspace for typical HDR-10 display operation.
%
% 'image' is an optional m-by-n-by-3 RGB or m-by-n-by-4 RGBA image matrix.
% If provided, the colorspace conversion will be applied to that image and
% the converted image will be returned as 2nd optional 'outImage' return
% argument.
%
% Return arguments:
%
% 'MCSC' is the 3-by-3 colorspace conversion matrix for going from the
% source colorspace to the target colorspace.
%
% 'outImage' is a converted version of the optional 'image' input image. If
% 'image' is omitted then 'outImage' is an [] empty matrix.
%

% History:
% 14-Oct-2020   mk  Written.

if nargin < 1 || isempty(srcGamut)
    error('srcGamut of source colorspace is missing.');
end

if ~isnumeric(srcGamut) || ~ismatrix(srcGamut) || ~isreal(srcGamut) || ~all(size(srcGamut) == [2, 4])
    error('srcGamut is not a 2-by-4 real matrix with red, green, blue, white-point 2D chroma coordinates as required.');
end

if nargin < 2 || isempty(dstGamut)
    error('dstGamut of destination color space is missing.');
end

% Onscreen window handle as 'dstGamut' provided?
if isscalar(dstGamut) && isnumeric(dstGamut) && isreal(dstGamut) && (Screen('WindowKind', dstGamut) == 1)
    % Yes. Query it for its assigned colorGamut, and use that:
    % The matrix is in proper 2x4 format.
    %
    % Note that the startup default for non-HDR windows is an all-zeros
    % matrix, signalling no color gamut being set:
    dstGamut = Screen('Hookfunction', dstGamut, 'WindowColorGamut');
else
    % Must be a 2-by-4 matrix with gamut info:
    if ~isnumeric(dstGamut) || ~ismatrix(dstGamut) || ~isreal(dstGamut) || ~all(size(dstGamut) == [2, 4])
        error('dstGamut is not a 2-by-4 real matrix with red, green, blue, white-point 2D chroma coordinates as required.');
    end
end

% srcGamut all zeros?
if ~any(srcGamut(:))
    error('srcGamut of source color space is an invalid all-zeros matrix.');
end

if nargin < 3 || isempty(image)
    image = [];
else
    % Validate image: Must be a m-by-n-by-3 or m-by-n-by-4 matrix for RGB or RGBA data:
    if ~isnumeric(image) || ~isreal(image) || ndims(image) ~= 3 || ~ismember(size(image, 3), [3, 4])
        error('image is not a m-by-n-by-3 or m-by-n-by-4 matrix for RGB or RGBA data as required.');
    end
end

% dstGamut all zeros?
if ~any(dstGamut(:))
    % Yes. That means gamut is undefined. Skip the whole transformation.
    MCSC = diag([1,1,1]);
    return;
end

% Convert gamuts into vectors [rx, ry, gx, gy, bx, by, wx, wy]:
dstGamut = dstGamut(:);
srcGamut = srcGamut(:);

% Actual defined source and destination gamut. Build CSC matrix via a
% detour to the XYZ color space:
rgbfromXYZ = XYZToRGBMatrix(dstGamut(1), dstGamut(2), dstGamut(3), dstGamut(4), dstGamut(5), dstGamut(6), dstGamut(7), dstGamut(8));
rgbtoXYZ = RGBToXYZMatrix(srcGamut(1), srcGamut(2), srcGamut(3), srcGamut(4), srcGamut(5), srcGamut(6), srcGamut(7), srcGamut(8));
MCSC = rgbfromXYZ * rgbtoXYZ;

% CSC of specific image wanted?
if ~isempty(image)
    % Seems legit. Do the image by matrix multiply for CSC:
    if size(image, 3) == 4
        % Need to keep alpha channel intact, so split up, do rgb,
        % reassemble:
        alpha = image(:,:,4);
        rgb = image(:,:,1:3);
        m = size(rgb, 1);
        n = size(rgb, 2);
        L = reshape(rgb, m * n, 3) * MCSC';
        rgb = reshape(L, m, n, 3);
        image = rgb;
        image(:,:,4) = alpha;
    else
        % RGB only, no alpha:
        rgb = image;
        m = size(rgb, 1);
        n = size(rgb, 2);
        L = reshape(rgb, m * n, 3) * MCSC';
        rgb = reshape(L, m, n, 3);
        image = rgb;
    end
end

return;
