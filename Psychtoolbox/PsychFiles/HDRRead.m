function [img, info, errmsg] = HDRRead(imgfilename, continueOnError, flipit)
% [img, info, errmsg] = HDRRead(imgfilename [, continueOnError=0][, flipit=0])
% Read a high dynamic range image file and return it as double() matrix,
% suitable for use with Screen('MakeTexture') and friends.
%
% Input arguments:
%
% 'imgfilename' - Filename of the HDR image file to load.
%
% 'continueOnError' Optional flag. If set to 1, HDRRead won't abort on
% error, but simply return an empty 'img' matrix and 'info' and a error
% message in 'errmsg'. Useful for probing if a specific file type is
% supported. If set to 0 or omitted, then HDRRead will abort with an error
% on any problem or unsupported image file types.
%
% 'flipit' Optional flag: If set to 1, the loaded image is flipped upside
% down.
%
% Return arguments:
%
% Returns 'img' - A double precision matrix of size h x w x c where h is
% the height of the input image, w is the width and c is the number of
% color channels: 1 for luminance images, 2 for luminance images with alpha
% channel, 3 for true-color RGB images, 4 for RGB images with alpha
% channel. If 'imgfilename' is not a supported file type or some error happens,
% then 'img' will be returned as empty [] matrix.
%
% Returns 'info' - A struct with info about the image. On error, 'info'
% will be returned as empty [] matrix. On success, 'info' has at least the
% following fields, which may be computed from information in the image file,
% or may be "made up" from internal hard-coded defaults, if the specific image
% file or the general image file format do not provide the information:
%
% info.format - A string describing the format of the image file, e.g.,
% 'rgbe' or 'openexr'. See below for supported formats and their id's.
%
% info.dataWindow = Data window [xmin, ymin, xmax, ymax] as defined by OpenEXR.
%
% info.displayWindow = Display window [xmin, ymin, xmax, ymax] as defined by OpenEXR.
%
% info.pixelAspectRatio = Pixel aspect ratio as defined by OpenEXR.
%
% info.screenWindowWidth = Window width as defined by OpenEXR.
%
% info.screenWindowCenter = [cx, cy] as defined by OpenEXR.
%
% info.lineOrder = Line order (0) = downwards/increasing (1) = upwards/decreasing
%                  as defined by OpenEXR.
%
% info.compression = Compression type id, as defined by OpenEXR: 0 = None,
%                    1 = RLE, 2 = ZIPS, 3 = ZIP, 4 = PIZ, 5 = PXR24, 6 = B44,
%                    7 = B44A, 128 = ZFP.
%
% info.GamutFromFile = 0 if the file did not provide color gamut information,
%                    = 1 if the file did provide color gamut information,
%                    = -1 if the file may or may not contain gamut information,
%                         but HDRRead() can not read it.
%
% info.ColorGamut = 2-by-4 matrix with the CIE 1931 2D chromaticity
%                   coordinates of the red, green, and blue primaries
%                   (column 1 - 3) and of the white-point (4th column),
%                   iow. the definition of the color gamut of the color
%                   space in which the image is represented.
%
%                   If info.GamutFromFile is 1, then this matrix is parsed
%                   from the image file. Otherwise, the file did not
%                   provide the info and a hard-coded default is returned,
%                   which is defined in the spec for the file format, e.g.,
%                   Rec-709 color space for OpenEXR images, and something
%                   similar for .hdr Radiance images.
%
% info.sampleToNits = Either a conversion factor from sample units to nits,
%                     ie. the value by which each color component needs to
%                     be multiplied to convert it into nits. Or the value
%                     zero, to mark the conversion factor as "unknown" if
%                     the file does not provide the conversion factor.
%
% Depending on the file format and the specific file, there may be more
% optional info.subfields available, with file format specific meaning. Not
% all image attributes can be parsed by HDRRead().
%
% Returns 'errmsg' - An empty string on success, but on failure may contain
% a useful error message for the user.
%
%
% HDRRead is a dispatcher for a collection of reading routines for
% different HDR image file formats. Currently supported are:
%
% * Radiance run length encoded RGBE format, read via read_rle_rgbe().
%   File extension is ".hdr" or ".pic". Returns a RGB image.
%
%   info.format is 'rgbe', color values are supposed to be in units of
%   radiance. The Radiance file format is specified here:
%
%   https://floyd.lbl.gov/radiance/refer/filefmts.pdf
%
%   The specification suggests that a pixel (r,g,b) color value of (1,1,1)
%   corresponds to a total energy of 1 watt/steradian/sq.meter over the
%   visible spectrum. It proposes the following formula for conversion to
%   luminance for the standard Radiance RGB primaries:
%
%   luminance = 179 * (0.265*R + 0.670*G + 0.065*B)
%
%   So (r,g,b) = (1,1,1) corresponds to white light of 179 nits luminance.
%   The value of 179 lumens/watt is the standard luminous efficacy of
%   equal-energy white light that is defined and used by Radiance
%   specifically for this conversion.
%
% * OpenEXR files, file extension is ".exr". info.format is 'openexr'.
%   By default, Screen()'s builtin Screen('ReadHDRImage') function is used,
%   which uses the builtin tinyexr open-source library from:
%
%   https://github.com/syoyo/tinyexr
%
%   This method is fast and can handle the most common OpenEXR format
%   encoding, single-part RGB(A) images, but will not be able to cope with
%   some more unusual encodings, e.g., YUV images, multipart images or deep
%   images, or additional integer channels for pixel ids. See the feature
%   table at tinyexr's GitHub page for features and limitations. Some of
%   these limitations apply due to Screen()'s current use of tinyexr, e.g.,
%   tinyexr can handle multi-part images and some deep images, but Screen
%   currently does not implement the needed interfaces.
%
%   A technical high level spec for OpenEXR files can be found at:
%
%   https://www.openexr.com/documentation/TechnicalIntroduction.pdf
%
%   In case of YUV images, HDRRead() will try to use the MIT licensed
%   exrread() command from the following 3rd package/webpage, if the
%   openexr-matlab package has been installed by the user:
%
%   https://github.com/skycaptain/openexr-matlab
%
%   That package uses the OpenEXR libraries for image reading, so those
%   libraries must be installed on your system as well by yourself. At
%   least GNU/Linux usually comes with libopenexr preinstalled or
%   installable from the distribution package archives.
%
%   The downside of using exrread() is that it won't provide color gamut
%   meta information, but always return fixed gamut info for a Rec-709
%   color space. For properly color-managed image reading you might
%   therefore be better off using a 3rd party OpenEXR converter application
%   to convert YUV images to RGB images, so Screen()'s internal .exr
%   reading function can be used.
%
% The reader routines are contributed code or open source / free software /
% public domain code downloaded from various locations under different, but
% MIT compatible licenses. See the help for the respective loaders for
% copyright and author information.
%

% History:
% 14-Dec-2006   mk Written.
% 21-Jul-2020   mk Updated for use with Psychtoolbox new HDR support.

psychlasterror('reset');

if nargin < 1 || isempty(imgfilename) || ~ischar(imgfilename)
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
img = [];
info = [];

if exist(imgfilename, 'file') ~= 2
    errmsg = ['HDR file ' imgfilename ' does not exist.'];

    if continueOnError
        warning(errmsg);
        return;
    else
        error(errmsg);
    end
end

[~, ~, fext] = fileparts(imgfilename);

if strcmpi(fext, '.hdr') || strcmpi(fext, '.pic')
    % Load a RLE encoded RGBE high dynamic range file:
    dispatched = 1;
    try
        % Use our own routine:
        [inimg, info] = read_rle_rgbe(imgfilename);

        % Primary and white-point CIE coordinates defined?
        if isfield(info, 'primaries')
            % Yes. Parse them into our standard ColorGamut format:
            info.GamutFromFile = 1;
            gamut = sscanf(info.primaries, '%f');
            info.ColorGamut = reshape(gamut, 2, 4);
        else
            % No. Assign default values per RADIANCE file format spec:
            info.GamutFromFile = 0;
            % These are almost like BT-709, but green x and white-point are
            % different:
            gamut = sscanf('0.640 0.330 0.2900.600 0.150 0.060 0.333 0.333', '%f');
            info.ColorGamut = reshape(gamut, 2, 4);
        end

        % Pixel aspect ratio:
        if isfield(info, 'pixaspect')
            info.pixelAspectRatio = sscanf(info.pixaspect, '%f');
        else
            info.pixelAspectRatio = 1.0;
        end

        % window width:
        info.screenWindowWidth = 1;

        % window center:
        info.screenWindowCenter = [0, 0];

        % dataWindow and displayWindow just represent width x height
        info.dataWindow = [0, 0, (info.width - 1), (info.height - 1)];
        info.displayWindow = info.dataWindow;

        % Line order equivalent:
        if info.Ysign == '-'
            info.lineOrder = 0;
        else
            info.lineOrder = 1;
        end

        % RLE compressed:
        info.compression = 1;

        % sampleToNits is unknown, so set it to zero:
        info.sampleToNits = 0;
    catch
        if continueOnError
            warning(['HDR file ' imgfilename ' failed to load.']);
            msg = psychlasterror;
            disp(msg.message);
            errmsg = ['Unknown error. Maybe: ' msg.message];
            return;
        else
            psychrethrow(psychlasterror);
        end
    end

    info.radianceformat = info.format;
    info.format = 'rgbe';
end

if strcmpi(fext, '.exr')
    % Load an OpenEXR high dynamic range file:
    dispatched = 1;
    try
        % Use our own Screen() implementation, which provides useful meta
        % data about color gamut and value to Nits mapping, but can neither
        % handle YUV images, nor multi-part images or deep images or some
        % other more exotic stuff:
        [inimg, format, err, info] = Screen('ReadHDRImage', imgfilename);
        if ~isempty(inimg)
            % Success. Assemble final info struct:
            info.format = format;
        elseif ~strcmp(err, 'R channel not found')
            % 'ReadHDRImage' failed for a reason other than that this is not
            % a RGB(A) image, but an YUV image. The optionally installed
            % exrread() command would not do better, as its only advantage
            % at the moment is that it can deal with single-part YUV
            % images. So this is a fail case:
            if continueOnError
                warning(['HDR file ' imgfilename ' failed to load: ' err]);
                msg = psychlasterror;
                disp(msg.message);
                errmsg = err;
                return;
            else
                psychrethrow(psychlasterror);
            end
        else
            % Failed because Red channel not found. Could be because it is
            % a YUV image, which our own implementation can not handle
            % atm., but exrread() can. Does (potentially optimized)
            % exrread() function exist?
            if exist('exrread', 'file')
                % Yes. Use it and give it another try:
                inimg = double(exrread(imgfilename));

                % If we made it to here without exception, then reading
                % worked, and we can try to also get some 'auxInfo':
                info = exrinfo(imgfilename);

                info.format = 'openexr';

                % First add optional keys, so we can override them below:
                for keyname = info.attributes.keys
                    ckey = char(keyname);
                    info = setfield(info, ckey, info.attributes(ckey)); %#ok<SFLD>
                end

                % Add the fields we promise will always be there for OpenEXR:
                t = info.attributes('dataWindow');
                info.dataWindow = double([t.min(1), t.min(2), t.max(1), t.max(2)]);

                t = info.attributes('displayWindow');
                info.displayWindow = double([t.min(1), t.min(2), t.max(1), t.max(2)]);

                t = info.attributes('screenWindowCenter');
                info.screenWindowCenter = double(t);

                info.pixelAspectRatio = double(info.attributes('pixelAspectRatio'));
                info.screenWindowWidth = double(info.attributes('screenWindowWidth'));

                if strcmpi(info.attributes('lineOrder'), 'increasing_y')
                    info.lineOrder = 0;
                elseif strcmpi(info.attributes('lineOrder'), 'decreasing_y')
                    info.lineOrder = 1;
                else
                    info.lineOrder = 2;
                end

                info.compression = info.attributes('compression');
                switch info.attributes('compression')
                    case 'none'
                        info.compression = 0;
                    case 'rle'
                        info.compression = 1;
                    case 'zips'
                        info.compression = 2;
                    case 'zip'
                        info.compression = 3;
                    case 'piz'
                        info.compression = 4;
                    case 'pxr24'
                        info.compression = 5;
                    case 'b44'
                        info.compression = 6;
                    case 'b44a'
                        info.compression = 7;
                end

                % exrinfo() can not provide gamut info, so pretend it
                % wasn't there in the file and assign the fallback default
                % of Rec-709. Is this a good idea? I don't know, but it is
                % the best we can do atm.:
                info.GamutFromFile = -1;
                info.ColorGamut = [0.6400, 0.3000, 0.1500, 0.3127 ; 0.3300, 0.6000, 0.0600, 0.3290];

                % If 'sampToNits' attribute exists, also return it as
                % sampleToNits, otherwise mark sampleToNits as "invalid":
                if ismember('sampToNits', info.attributes.keys)
                    info.sampleToNits = double(info.attributes('sampToNits'));
                else
                    info.sampleToNits = 0;
                end
            else
                % No. Maybe clue the user in on exrread() by abusing the
                % exception handling (catch) below?
                error('Could not read image, probably because it is a YUV image and the optional exrread() command is not installed, which could possibly handle it.');
            end
        end
    catch
        if continueOnError
            warning(['HDR file ' imgfilename ' failed to load.']);
            msg = psychlasterror;
            disp(msg.message);
            errmsg = ['Unknown error. Maybe: ' msg.message];
            return;
        else
            psychrethrow(psychlasterror);
        end
    end

    info.format = 'openexr';
end

if dispatched == 0
    if ~continueOnError
        error(['Potential HDR file ' imgfilename ' is of unknown type, or no loader available for this type.']);
    else
        warning(['Potential HDR file ' imgfilename ' is of unknown type, or no loader available for this type']);
        errmsg = 'No loader available for this type.';

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

errmsg = '';

% Done.
return;
