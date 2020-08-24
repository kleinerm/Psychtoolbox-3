function [img, format] = HDRRead(imgfilename, continueOnError, flipit)
% [img, format] = HDRRead(imgfilename[, continueOnError=0][, flipit=0])
% Read a high dynamic range image file and return it as Matlab double
% matrix, suitable for use with Screen('MakeTexture') and friends.
%
% 'imgfilename' - Filename of the HDR image file to load.
% Returns 'img' - A double precision matrix of size h x w x c where h is
% the height of the input image, w is the width and c is the number of
% color channels: 1 for luminance images, 2 for luminance images with alpha
% channel, 3 for true-color RGB images, 4 for RGB images with alpha
% channel.
%
% 'continueOnError' Optional flag. If set to 1, HDRRead won't abort on
% error, but simply return an empty img matrix. Useful for probing if a
% specific file type is supported.
%
% 'flipit' Optional flag: If set to 1, the loaded image is flipped upside
% down.
%
% Returns a double() precision image matrix in 'img', and a id in 'format' which
% describes the format of the source image file:
%
% 'rgbe' == Radiance RGBE format: Color values are in units of radiance.
% 'openexr' == OpenEXR image format.
%
% HDRRead is a dispatcher for a collection of reading routines for
% different HDR image file formats. Currently supported are:
%
% * Run length encoded RGBE format, read via read_rle_rgbe.m, extension is
% ".hdr". Returns a RGB image.
%
% * OpenEXR files, extension is ".exr". This are readable efficiently if
% the MIT licensed exrread() command from the following package/webpage is
% installed: https://github.com/skycaptain/openexr-matlab
%
% The reader routines are contributed code or open source / free software /
% public domain code downloaded from various locations under different, but
% MIT compatible licenses. See the help for the respective loaders for
% copyright and author information.

% History:
% 14-Dec-2006   mk Written.
% 21-Jul-2020   mk Updated for use with Psychtoolbox new HDR support.

if nargin < 1 || isempty(imgfilename)
    error('Missing HDR image filename.');
end

if nargin < 2 || isempty(continueOnError)
    continueOnError = 0;
end

if nargin < 3 || isempty(flipit)
    flipit = 0;
end

% Format dispatcher:
dispatched = 0;

if ~isempty(strfind(imgfilename, '.hdr')) %#ok<STREMP>
    % Load a RLE encoded RGBE high dynamic range file:
    dispatched = 1;
    try
        % Does (potentially optimized) hdrread() function exist?
        if exist('hdrread', 'file')
            % Use it.
            inimg = double(hdrread(imgfilename));
        else
            % Use our own fallback:
            inimg = read_rle_rgbe(imgfilename);
        end
    catch
        if continueOnError
            img = [];
            warning(['HDR file ' imgfilename ' failed to load.']);
            msg = psychlasterror;
            disp(msg.message);
            return;
        else
            psychrethrow(psychlasterror);
        end
    end

    format = 'rgbe';
end

if ~isempty(strfind(imgfilename, '.exr')) %#ok<STREMP>
    % Load an OpenEXR high dynamic range file:
    dispatched = 1;
    try
        % Does (potentially optimized) hdrread() function exist?
        if exist('exrread', 'file')
            % Use it.
            inimg = double(exrread(imgfilename));
        else
            % Use our own fallback:
            % TODO
            dispatched = 0;
        end
    catch
        if continueOnError
            img = [];
            warning(['HDR file ' imgfilename ' failed to load.']);
            msg = psychlasterror;
            disp(msg.message);
            return;
        else
            psychrethrow(psychlasterror);
        end
    end

    format = 'openexr';
end

if dispatched == 0
    format = [];

    if ~continueOnError
        error(['HDR file ' imgfilename ' is of unknown type. No loader available.']);
    else
        img = [];
        warning(['HDR file ' imgfilename ' is of unknown type. No loader available.']);
        return;
    end
end

if flipit
    img(:,:,1) = flipud(inimg(:,:,1));
    img(:,:,2) = flipud(inimg(:,:,2));
    img(:,:,3) = flipud(inimg(:,:,3));
else
    img = inimg;
end

% Done.
return;
