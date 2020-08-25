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
%    image files as Matlab double() precision matrices.
%
% 3. Set the optional 'floatprecision' flag of Screen('MakeTexture', ...) to 1 or 2
%    to enforce creation of floating point precision HDR textures from your image matrix.
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
    % No: Try some default file:
    if ~IsOctave
        % Matlab comes with exactly one HDR sample file in radiance format:
        imfilename = [matlabroot filesep 'toolbox/images/imdata/office.hdr'];
    else
        % Octave doesn not ship with a HDR sample image, but try if this is
        % the development machine which happens to have Matlab installed in a
        % peculiar location:
        imfilename = '/home/shared/MATLAB/R2019a/toolbox/images/imdata/office.hdr';
    end
end

% Does imfilename exist?
if ~exist(imfilename, 'file')
    error('Specified HDR image file does not exist under path %s.', imfilename);
end

% Read HDR file. Must be in a file format recognized by HDRRead:
[img, hdrType] = HDRRead(imfilename);

switch hdrType
    case 'rgbe'
        % HACK: Multiply by 180.0 as a crude approximation of Radiance units to nits:
        % This is not strictly correct, but will do to get a nice enough picture for
        % demo purpose:
        img = img * 180;

    otherwise
        error('Unknown image format. Do not know how to convert into units of Nits.');
end

% Compute maximum and max mean luminance of the image:
maxCLL = max(max(max(img))) %#ok<NOPRT>
maxFALL = mean(mean(mean(img))) %#ok<NOPRT>

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

% Tell the HDR display about maximum frame average light level and maximum
% content light level of the image:
PsychHDR('HDRMetadata', win, [], maxFALL, maxCLL);

% Build a Psychtoolbox 16 bpc half-float texture from the image array
% by setting the (optional) 'floatprecision' flag to 1. If you need
% even more precision you can provide the value 2 instead of 1,
% creating full 32 bpc float textures. These will take up twice the
% amount of memory and bandwidth though:
texid = Screen('MakeTexture', win, img, [], [], 1);

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
fprintf('Displayed %i frames in %f seconds, for an average framerate of %f fps.\n', framecounter, duration, averagefps);
fprintf('Bye bye!\n');
