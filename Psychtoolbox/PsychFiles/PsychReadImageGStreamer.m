function img = PsychReadImageGStreamer(imagefile, win, uselowprecision)
% img = PsychReadImageGStreamer(imagefile, win [, uselowprecision=0])
%
% Caution: Experimental implementation - not much tested yet. Here be dragons!
%
% Reads an image file (or URL) by (ab)using GStreamer's ability to read image
% files as if they were movies. Returns double() or uint8() RGB image matrix
% with decoded image in 'img' if the image file format is supported by GStreamer.
%
% This is mostly meant as a fallback for reading exotic image files unsupported
% by imread() or HDR files unsupported by our other HDR reading routines. In the
% latter case, usefulness might be limited as of GStreamer 1.18, as GStreamer
% only supports unorm value range with up to 16 bpc for some formats, but not
% the out-of-unorm float range often needed for HDR imaging data. Might still be
% useful though for some high-precision unorm range formats.
%
% 'imagefile' Name or URL of the image file to load.
%
% 'win' Onscreen window handle, as we abuse Screen()'s movie playback functions
% for this, and they always need an onscreen window handle to work.
%
% 'uselowprecision' Optional: If set to 1, return uint8() data RGB8, otherwise
% return high precision double() matrix which contains single precision float
% RGB image data, enough to cover 16 bpc unorm data and up to 32 bpc float data.
%

% History:
% 28-Aug-2020   mk  Written.

if nargin < 1 || isempty(imagefile)
    error('Required filename of image file missing.');
end

if nargin < 2 || isempty(win) || Screen('WindowKind', win) ~= 1
    error('Required onscreen window handle missing.');
end

if nargin < 3 || isempty(uselowprecision) || uselowprecision ~= 1
    % Request format 10: RGBA64 64 bpp, 16 bpc RGBA:
    pixelFormat = 10;

    % Fetch into 32 bpc single precision texture to retain full potential 16 bpc
    % unorm fixed point precision:
    floatprecision = 2;
else
    % Use low precision 8 bpc unorm:
    pixelFormat = [];
    floatprecision = 0;
end

verbosity = Screen('Preference', 'Verbosity', 1);
try
    [movie, ~, ~, imgw, imgh] = Screen('OpenMovie', win, imagefile, [], [], [], pixelFormat);
    Screen('Preference', 'Verbosity', verbosity);
catch
    Screen('Preference', 'Verbosity', verbosity);
    error(['Could not load image file ' imagefile ' See GStreamer error messages above.']);
end

% Need this to avoid timeout error during 1st (and only) frame fetch:
Screen('PlayMovie', movie, 1, 0, 0);

% Get image as texture:
tex = Screen('GetMovieImage', win, movie, 1);

% Retrieve as double() or uint8() matrix img with 3 layers RGB:
img = Screen('GetImage', tex, [], [], floatprecision, 3);

% Dispose texture, close movie:
Screen('Close', tex);
Screen('CloseMovie', movie);

% Return double() or uint8() RGB 3 layer image matrix in img:
return;
