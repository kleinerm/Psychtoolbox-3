function SimpleHDRLinuxStereoDemo(imfilename)
% SimpleHDRLinuxStereoDemo([imfilename]) - Load and show a HDR image on a compatible
% HDR stereo display setup.
%
% Press any key to terminate the demo.
%
% 'imfilename' - Optional filename of the HDR image to load. This will load
% a standard HDR demo image bundled with Matlab, if omitted, or abort if
% the default demo image is missing. Currently only '.hdr' RGBE images are
% supported.
%
% See SimpleHDRDemo for explanations. This is the same demos, but displaying
% stereoscopically on Linux + X11 via the special Linux hack enabled by the
% Linux only PsychImaging task
%
% PsychImaging('AddTask', 'General', 'UseStaticHDRHack', hdrMeta);
%
% See help PsychImaging in the 'UseStaticHDRHack' section for explanations,
% background info and setup instructions.
%

% History:
%
% 18-Dec-2020   mk  Written. Derived from SimpleHDRDemo.

if ~IsLinux || IsWayland
    fprintf('Sorry, this demo only works on Linux/X11, not under Wayland or other operating systems.\n');
    return;
end

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

% Define static HDR metadata to use for the whole duration of this demo session:
hdrMeta.MetadataType = 0;
hdrMeta.MinLuminance = 0.050;
hdrMeta.MaxLuminance = 600;
hdrMeta.ColorGamut = [0.6400, 0.3000, 0.1500, 0.3127 ; 0.3300, 0.6000, 0.0600, 0.3290];

% Convert img from its source colorspace to the display colorspace defined for
% the mastering display. info.ColorGamut is the color gamut parsed from the
% image file, or a default color gamut as mandated by the image file format
% spec for the image file:
[foo, img] = ConvertRGBSourceToRGBTargetColorSpace(info.ColorGamut, hdrMeta.ColorGamut, img);

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

% Compute and assign maximum and max mean luminance of the image:
[maxFALL, maxCLL] = ComputeHDRStaticMetadataType1ContentLightLevels(img);
hdrMeta.MaxFrameAverageLightLevel = maxFALL;
hdrMeta.MaxContentLightLevel = maxCLL;

% Open a double-buffered fullscreen onscreen stereo window in HDR mode on the HDR
% display, with black background color. Color values will be specified in
% units of nits, the display is done according to HDR10 standard, ie.
% Color space is BT2020, SMPTE ST-2084 PQ encoding is used to drive the
% display, output color signals have 10 bpc precision. Use a dual-display or
% side-by-side single display stereo configuration:
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'EnableHDR', 'Nits', 'HDR10'); %, 'Dummy');
PsychImaging('AddTask', 'General', 'UseStaticHDRHack', hdrMeta);
win = PsychImaging('OpenWindow', screenid, 0, [], [], [], 4);
HideCursor(win);

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
    % Draw left eye stim:
    Screen('SelectStereoDrawBuffer', win, 0);

    % Draw our HDR texture at specified rotation angle, centered in the window:
    Screen('DrawTexture', win, texid, [], [], rotAngle);

    % Draw some 2D filled oval, with a bounding box of [left,top,right,bottom]
    % = [500 500 600 600], and a color value of [R, G, B] = [300 nits, 300 nits, 0 nits],
    % ie. a yellow oval with 300 nits:
    Screen('FillOval', win, [300 300 0], [500 500 600 600]);

    % Draw right eye stim:
    Screen('SelectStereoDrawBuffer', win, 1);

    % Draw our HDR texture at specified rotation angle, centered in the window:
    Screen('DrawTexture', win, texid, [], [], rotAngle);

    % Draw some 2D filled oval, with a bounding box of [left,top,right,bottom]
    % = [520 500 620 600], and a color value of [R, G, B] = [300 nits, 300 nits, 0 nits],
    % ie. a yellow oval with 300 nits. This is shifted 40 pixels to the left, wrt.
    % the otherwise identical left-eye stimulus to create a teeny tiny bit of stereo:
    Screen('FillOval', win, [300 300 0], [460 500 560 600]);

    % Show updated HDR framebuffer at next vertical retrace:
    vbl = Screen('Flip', win, vbl);

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
fprintf('Displayed %i stereo-frames in %f seconds, for an average framerate of %f fps.\n', framecounter, duration, averagefps);
fprintf('Bye bye!\n');
