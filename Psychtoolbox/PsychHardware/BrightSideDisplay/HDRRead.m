function img = HDRRead(imgfilename, continueOnError, flipit)
% img = HDRRead(imgfilename[, continueOnError=0][, flipit=0])
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
% HDRRead is a dispatcher for a collection of reading routines for
% different HDR image file formats. Currently supported are:
%
% * Run length encoded RGBE format, read via read_rle_rgbe.m, extension is
% ".hdr". Returns a RGB image.
%
% The reader routines are contributed code or open source / free software /
% public domain code downloaded from various locations under different, but
% GPL compatible licenses. See the help for the respective loaders for
% copyright and author information.

% History:
% 12/14/06 Written (MK).

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

if ~isempty(findstr(imgfilename, '.hdr'))
    % Load a RLE encoded RGBE high dynamic range file:
    dispatched = 1;
    try
        inimg = read_rle_rgbe(imgfilename);
        if flipit
            img(:,:,1) = flipud(inimg(:,:,1));
            img(:,:,2) = flipud(inimg(:,:,2));
            img(:,:,3) = flipud(inimg(:,:,3));
        else
            img(:,:,1) = inimg(:,:,1);
            img(:,:,2) = inimg(:,:,2);
            img(:,:,3) = inimg(:,:,3);
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
end

if dispatched == 0
    if ~continueOnError
        error(['HDR file ' imgfilename ' is of unknown type. No loader available.']);
    else
        img = [];
        warning(['HDR file ' imgfilename ' is of unknown type. No loader available.']);
        return;
    end
end

% Done.
return;
