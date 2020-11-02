function [safemaxFALL, safemaxCLL, maxFALL, maxCLL, errmsg] = ComputeHDRStaticMetadataType1ContentLightLevels(images, integerScalesToSDRRange, nanflag, continueOnError)
% [safemaxFALL, safemaxCLL, maxFALL, maxCLL, errmsg] = ComputeHDRStaticMetadataType1ContentLightLevels(images [, integerScalesToSDRRange=80Nits][, nanflag='omitnan'][, continueOnError=0]);
%
% Compute CTA-861-G static content light levels for an image or a cell array of images.
%
% Compute the maximum frame average light level in nits and return it in
% 'maxFALL'. Compute the maximum content light level in nits and return it
% in 'maxCLL'. Return diagnostic error messages when appropriate in
% 'errmsg', or an empty 'errmsg' if there weren't any notable issues.
% 'safemaxFALL' and 'safemaxCLL' are clamped versions of 'maxFALL' and
% 'maxCLL', ie. as long as calculated values are within the specified range
% for HDR static metadata type 1, they are identical. Otherwise, the safe
% variants are clamped to the limits of the safe range, ie. the range from
% 0 to 65535 nits. NaN values for maxFALL or maxCLL result in safemaxFALL
% or safemaxCLL values of zero, as these signal "unknown" according to the
% standard.
%
% The returned maxFALL and maxCLL values are computed according to standard
% CTA-861-G, Annex P. 'maxCLL' the "maximum content light level" is defined
% as the luminance value in units of nits of the brightest pixel color
% component over all pixels in all frames of the image sequence. 'maxFALL'
% the "maximum frame average light level" is defined as the maximum over
% the average luminance values of all frames in the image sequence, where
% the average luminance of each frame is calculated over all pixels, but
% for each pixel only the value of the brightest color component is taken
% into account for calculation of the average, not the combined luminance
% of the whole pixel! One consequence of these definitions is that the computed
% maxFALL, maxCLL luminance values are only strictly correct under the
% assumption of an achromatic pixel, where the chromaticity coordinates of
% the pixel would correspond to those of the white-point of the display.
% Therefore, for an actual color image, maxFALL and maxCLL as measured by a
% colorimeter would likely be (much) lower than the maxFALL and maxCLL
% calculated by this function according to the standard.
%
% The maxFALL and maxCLL values calculated here can be used as arguments to
% a call to PsychHDR('HDRMetadata', ...) to define what is transmitted to a
% connected HDR display device. They are meant as guidance and conservative
% estimates of what content light levels to expect, to aid potential vendor
% and display-model specific algorithms for tone-mapping and thermal-/power-/
% backlight-dimming management which may be implemented in the display device.
% Those algorithms try to reproduce HDR content which is outside the
% operating range of the display device in a faithful manner, using maxCLL,
% maxFALL and other static HDR metadata to help with the tradeoff between
% proper image reproduction and not damaging or overheating the display device.
%
%
% Input arguments:
%
% 'images' can be a single image matrix of height-by-width-by-channels
% pixel color components, for a height-by-width pixels image, with channels
% being 1 for a luminance image, 2 for a luminance + alpha channel image, 3
% for a RGB color image, or 4 for a RGBA image with additional alpha
% channel. The alpha channel of a 2 channel matrix or a 4 channel matrix is
% ignored for light level computations. 'images' will usually be a floating
% point HDR image, but integer images are also accepted, in which case
% returned 'maxFALL' and 'maxCLL' values are adapted according to the
% optional parameter 'integerScalesToSDRRange'. 'images' can also be a
% cell() array of image matrices, representing a whole image sequence. In
% case of a cell array, each image in the array contributes to light level
% property calculations and you get the maxCLL over the whole sequence, and
% maxFALL over the whole sequence.
%
% 'integerScalesToSDRRange' Optional: How to treat 'images' of integer
% type, instead of floating point type. Currently the Screen('MakeTexture')
% command for turning image matrices into displayable image textures only
% accepts integer image matrices of type uint8, and floating point matrices
% of type double(). double() matrices are used "as is" for HDR display.
%
% Integer uint8 matrices are by default converted into HDR images with a
% value range that corresponds to typical standard dynamic range (SDR).
% Values from 0 to 255 are assumed to represent the HDR subrange [0; 80]
% nits. Iow. by default 0 maps to 0 nits and the maximum uint8 value of
% 255 maps to 80 nits. See the help text of "Screen MakeTexture?" and for
% the PsychImaging() HDR setup functions for details on how this behaviour
% can be customized. 'integerScalesToSDRRange' defines how to adapt the
% returned 'maxFALL' and 'maxCLL' values if a integer image matrix is
% passed in: A value of 0 will just treat it like a floating point matrix.
% A value > 0 will map the possible value range in the integer matrix to
% the range 0 - integerScalesToSDRRange. The default setting if this
% parameter is omitted, is to map the integer range to a range of 0 - 80
% nits, iow. returned maxCLL and maxFALL will be in the range 0 - 80 nits.
% This is the most reasonable behaviour if HDR display mode is requested
% with all parameters at their default and Screen('MakeTexture') is used
% with all precision related parameters at their defaults.
%
% 'nanflag' Optional: How to treat NaN "not a number" values in pixel color
% components: By default 'nanflag' is 'omitnan' - Only non-NaN elements
% contribute to maximum and mean calculations for maxCLL, maxFALL. The
% other valid setting would be 'includenan' - If any pixel color component
% in any of the passed in 'images' is NaN, the returned maxCLL and maxFALL
% will be NaN.
%
% 'continueOnError' Optional: If set to 0 (the default), abort with an
% error message on invalid input. If set to 1, try to continue if possible,
% just printing a warning message and return a user comprehensible error
% string in 'errmsg'.
%

% History:
% 24-Sep-2020   mk  Written.

% Default to no error:
errmsg = [];

if nargin < 1 || isempty(images)
    error('None, or empty images parameter provided instead of image matrix or cell array of images.');
end

% If omitted, integer images [0 ; maximum integer value] shall be mapped to
% [0 ; 80 nits], as this is the default of PsychHDR() for setting up HDR
% display modes if all optional parameters are omitted:
if nargin < 2 || isempty(integerScalesToSDRRange)
    integerScalesToSDRRange = 80;
else
    if ~isscalar(integerScalesToSDRRange) || ~isnumeric(integerScalesToSDRRange) || ~isreal(integerScalesToSDRRange) || integerScalesToSDRRange < 0
        error('Invalid integerScalesToSDRRange parameter specified. Not a scalar greater than or equal to zero.');
    end
end

% Omit NaN values in all calculations by default:
if nargin < 3 || isempty(nanflag)
    nanflag='omitnan';
else
    switch nanflag
        case {'omitnan', 'includenan'}
            % All good, nothing to do.
        otherwise
            error('Invalid nanflag provided.');
    end
end

if nargin < 4 || isempty(continueOnError)
    continueOnError = 0;
else
    if ~ismember(continueOnError, [0, 1])
        error('Invalid continueOnError flag specified. Not 0 or 1.');
    end
end

% maxCLL is value of the brightest pixel color component of the brightest
% pixel over all images:
if iscell(images)
    % Cell array with each cell containing one image. Iterate over all of
    % them and keep track of global sequence maxCLL, maxFALL:
    maxCLL = 0;
    maxFALL = 0;

    for i=1:length(images)
        [curCLL, curFALL, msg] = computeLLProps(images{i}, integerScalesToSDRRange, nanflag, continueOnError, i);
        if (isnan(curCLL) || isnan(curFALL)) && strcmp(nanflag, 'includenan')
            % NaN's found in at least one image, and supposed to return NaN
            % for whole sequence, do it, we're done here:
            maxCLL = nan;
            maxFALL = nan;
            % NaN's are not safe for use in PsychHDR(). Choose 0 as the
            % defined "i don't know" value for the HDR monitor:
            safemaxFALL = 0;
            safemaxCLL = 0;
            errmsg = msg;
            return;
        end

        maxFALL = max(maxFALL, curFALL);
        maxCLL = max(maxCLL, curCLL);
        if ~isempty(msg)
            errmsg = [errmsg, ' : ', msg]; %#ok<AGROW>
        end
    end
else
    % Single image matrix: Directly compute props of the one image:
    [maxCLL, maxFALL, errmsg] = computeLLProps(images, integerScalesToSDRRange, nanflag, continueOnError, 1);
end

% Clamp safe (for PsychHDR to transmit HDR metadata) values to allowable
% range of 0 - 65535 nits:
safemaxFALL = min(max(maxFALL, 0), 65535);
safemaxCLL = min(max(maxCLL, 0), 65535);

% Return final maxFALL, maxCLL:
return;

% Compute light level props of one image matrix:
function [maxCLL, FALL, errmsg] = computeLLProps(img, integerScalesToSDRRange, nanflag, continueOnError, index)
    % Safe return values on error:
    errmsg = [];
    maxCLL = nan;
    FALL = nan;

    % Validate size and dimensions of the image:
    if ~ismember(ndims(img), [2, 3]) || size(img, 1) < 1 || size(img, 2) < 1 || ~ismember(size(img, 3), [1,2,3,4])
        errmsg = sprintf('At least one provided image (index %i) is not a valid 2D or 3D image matrix of minimum size 1-by-1 pixel, with 1, 2, 3 or 4 color channels.', index);
        if continueOnError
            warning(errmsg); %#ok<*SPWRN>
            return;
        else
            error(errmsg); %#ok<*SPERR>
        end
    end

    % Is it a numeric matrix?
    if ~isnumeric(img) || ~isreal(img)
        errmsg = sprintf('At least one provided image (index %i) is not a valid numeric 2D or 3D image matrix.', index);
        if continueOnError
            warning(errmsg); %#ok<*SPWRN>
            return;
        else
            error(errmsg); %#ok<*SPERR>
        end
    end

    % Throw away alpha channel of 2 layer LA and 4 layer RGBA images:
    if ismember(size(img, 3), [1,3])
        img = img(:,:,:);
    else
        img = img(:,:,1:end-1);
    end

    % Want to include NaN's, ie. return NaN if any elements are NaN?
    if strcmp(nanflag, 'includenan') && any(isnan(img(:)))
        % At least one image pixel component somewhere is NaN. Return NaN:
        maxCLL = nan;
        FALL = nan;
        return;
    end

    % At this point, either img does not contain NaN's, only valid values,
    % or the nanflag is 'omitnan', so we should compute stuff, but ignore
    % any NaN elements for the computations.
    % Matlab and Octave default behaviour for max() is to ignore NaN's for
    % maximum search, exactly what we want.
    % Fixed Octave behaviour for mean() is to return NaN if anything is
    % NaN, therefore we must make sure that at least for mean()
    % computations we will have scrubbed the input from NaN's.

    % maxCLL is the maximum over all color components of all pixels in the
    % luminance layer or RGB layers of the image:
    maxCLL = max(img(:));

    % FALL is the average over all pixels in the image, but for each pixel,
    % only the pixel color component with maximum value will contribute to
    % the average, so blue dominated regions would use the blue color
    % channel values to participate in the mean computation, whereas in
    % green dominated regions, only green components would be used:
    %
    % First build a single channel image, which contains the maximum
    % component value for each pixel, iow. each entry (y,x) is the maximum
    % over the color components of pixel (y,x). For a pure luminance
    % matrix, maximg is naturally == img:
    maximg = max(img, [], 3);

    % Now compute the arithmetic mean over all values in 2D matrix maximg
    % which are not NaN:
    FALL = mean(maximg(~isnan(maximg(:))));

    % Make sure we only output double() values, as expected for further
    % math with high precision, and especially for PsychHDR('HDRMetaData', ...)
    % arguments:
    maxCLL = double(maxCLL);
    FALL = double(FALL);

    % uint image instead of float image, and treatment as SDR requested?
    if integerScalesToSDRRange && ~isfloat(img)
        % Yes: Values [0 ; intmax(class(img))] are mapped to [0; 1] unorm
        % range by Psychtoolbox/Screen(), which is in turn assumed to be
        % mapped to [0; integerScalesToSDRRange] for HDR onscreen windows:
        integerScalesToSDRRange = double(integerScalesToSDRRange) / double(intmax(class(img)));
        maxCLL = maxCLL * integerScalesToSDRRange;
        FALL = FALL * integerScalesToSDRRange;
    end

return;
