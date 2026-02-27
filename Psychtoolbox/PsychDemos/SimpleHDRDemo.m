function SimpleHDRDemo(imfilename)
% SimpleHDRDemo([imfilename]) - Load and show a high dynamic range (HDR) image
% on a compatible HDR display setup.
%
% Press any key to terminate the demo.
%
% 'imfilename' - Optional filename of the HDR image to load. This will load
% a standard HDR demo image bundled with Matlab, if omitted, or abort if
% the default demo image is missing. Currently only '.hdr' RGBE images are
% supported.
%
% See "help PsychHDR" for system requirements and setup instructions for HDR
% display. Once these are satisfied, converting a standard Psychtoolbox visual
% stimulation script into a HDR script is straightforward, as shown in this simple
% demo. Modify your scripts in the following manner:
%
% 1. Use (as most minimal setup) the sequence ...
%
%    PsychImaging('PrepareConfiguration');
%    PsychImaging('AddTask', 'General', 'EnableHDR');
%    win = PsychImaging('OpenWindow', screenid);
%
%    ... instead of ...
%
%    win = Screen('OpenWindow', screenid);
%
%    ... to open a fullscreen onscreen window on a HDR capable display device,
%    which is attached to a HDR capable graphics card.
%
% 2. Use HDRRead(imfilename) instead of imread(imfilename) to load HDR
%    image files as double() precision matrices.
%
% 3. Optional: Set the optional 'floatprecision' flag of Screen('MakeTexture', ...)
%    to 1 or 2 to enforce creation of floating point precision HDR textures
%    of a specific precision from your image matrix.
%
%    By default, 'floatprecision' will be selected as 2 for single
%    precision float fp32 format, which is the maximum precision for
%    processing and displaying HDR images.
%
%
% See the section 'EnableHDR' of "help PsychImaging" for more optional parameters
% to pass to PsychImaging('AddTask', 'General', 'EnableHDR'); for customizing the
% HDR display mode. See "help PsychHDR" for more helper subfunctions to customize
% HDR display further at runtime, once the fullscreen onscreen HDR display window
% has been opened and initially set up by PsychImaging('AddTask', 'General', 'EnableHDR').
%

% History:
%
% 21-Jul-2020   mk  Written.

% Make sure we run on Psychtoolbox-3. Abort otherwise. Use unified key names for
% keyboard input across all supported operating systems. Use normalized color range,
% not the old 0-255 8 bit color convention:
PsychDefaultSetup(2);

% imfilename provided?
if nargin < 1 || isempty(imfilename)
    % No: Try some default file from our bundled OpenEXR sample images:
    imfilename = [PsychtoolboxRoot 'PsychDemos/OpenEXRImages/Desk.exr'];
end

% Does imfilename exist?
if ~exist(imfilename, 'file')
    error('Specified HDR image file does not exist under path %s.', imfilename);
end

% Read HDR file, abort on error. Must be in a file format recognized by HDRRead:
[img, info] = HDRRead(imfilename);

% Show metadata:
disp(info);

% Find screen to display on: We choose the one with the highest number,
% assuming this is the HDR display:
screenid = max(Screen('Screens'));

% Set a cleanup function: If the variable 'canary' goes out of scope due to
% script termination (error or user abort), call "sca" which will close the display:
canary = onCleanup(@sca);

% Open a double-buffered fullscreen onscreen window in HDR mode on the HDR
% display, with black background color. Color values will be specified in
% units of nits, the display is done according to HDR10 standard, ie.
% Color space is BT2020, SMPTE ST-2084 PQ encoding is used to drive the
% display, output color signals have 10 bpc precision.
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'EnableHDR', 'Nits', 'HDR10');
% Note: This would also work, as above settings are used by default:
% PsychImaging('AddTask', 'General', 'EnableHDR');
win = PsychImaging('OpenWindow', screenid, 0);
HideCursor(win);

% Convert img from its source colorspace to the display colorspace of the
% HDR onscreen window. info.ColorGamut is the color gamut parsed from the
% image file, or a default color gamut as mandated by the image file format
% spec for the image file. win is the onscreen window handle, and the
% function will query win for the color gamut of its associated colorspace:
[~, img] = ConvertRGBSourceToRGBTargetColorSpace(info.ColorGamut, win, img);

switch info.format
    case 'rgbe'
        % HACK: Multiply by 179.0 as a crude approximation of Radiance
        % units to nits: This is not strictly correct, but will do to get a
        % nice enough picture for demo purpose. See 'help HDRRead' for the
        % motivation for the 179 multiplicator for Radiance images:
        % This is always RGB, no alpha channel to deal with.
        img = img * 179;

    case 'openexr'
        % Known scaling factor for scaling pixel values into units of nits?
        % Otherwise it is best to just leave it as is:
        if info.sampleToNits > 0
            % Only scale RGB channels, not the alpha channel.
            img(:,:,1:3) = img(:,:,1:3) * info.sampleToNits;
        end

    otherwise
        error('Unknown image format. Do not know how to convert into units of Nits.');
end

% Compute maximum and max mean luminance of the image:
[maxFALL, maxCLL] = ComputeHDRStaticMetadataType1ContentLightLevels(img);

% Tell the HDR display about maximum frame average light level and maximum
% content light level of the image:
PsychHDR('HDRMetadata', win, 0, maxFALL, maxCLL);

% Build a 32 bpc single-precision float texture from the image
% array by setting the (optional) 'floatprecision' flag to 2.
% texid = Screen('MakeTexture', win, img, [], [], 2);
%
% Or simply don't, because the 'floatprecision' flag defaults to 2
% in HDR display mode anyway if omitted, for your convenience:
texid = Screen('MakeTexture', win, img);

% Some variable rotation angle for the image, for some simplistic animation:
rotAngle = 0;

% Some framecounter for stats:
framecounter = 0;

% Initial Flip to sync us to retrace:
vbl = Screen('Flip', win);
tstart = vbl;

% Animation loop: Show a rotating HDR image, until key press:
while ~KbCheck
    % Draw our HDR texture at specified rotation angle, centered in the window:
    Screen('DrawTexture', win, texid, [], [], rotAngle);

    % Draw some 2D filled oval, with a bounding box of [left,top,right,bottom]
    % = [500 500 600 600], and a color value of [R, G, B] = [300 nits, 300 nits, 0 nits],
    % ie. a yellow oval with 300 nits:
    Screen('FillOval', win, [300 300 0], [500 500 600 600]);

    % And some text, 30 pixels high, centered, in a 200 nits green:
    Screen('TextSize', win, 30);
    DrawFormattedText(win, 'If it works, it works.\nIf it doesn''t, it doesn''t.\n(Quoc Vuong, 2006)', 'center', 'center', [0 200 0]);

    % Show updated HDR framebuffer at next vertical retrace:
    vbl = Screen('Flip', win);

    % Increase rotation angle to make it a bit more interesting:
    rotAngle = rotAngle + 0.1;

    % Count our frames:
    framecounter = framecounter + 1;
end

% We are done. Release all textures, close all windows, shutdown:
sca;

% Print the stats:
duration = vbl - tstart;
averagefps = framecounter / duration;
fprintf('Displayed %i frames in %f seconds, for an average framerate of %f fps.\n', framecounter, duration, averagefps);
fprintf('Bye bye!\n');
