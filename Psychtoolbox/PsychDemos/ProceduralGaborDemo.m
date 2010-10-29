function ProceduralGaborDemo(benchmark, nonsymmetric)
% ProceduralGaborDemo([benchmark=0][, nonsymmetric=0])
%
% This demo demonstrates fast drawing of Gabor patches via use procedural
% texture mapping. It only works on hardware with support for the GLSL
% shading language, vertex- and fragment-shaders.
%
% Gabors are not encoded into a texture, but instead a little algorithm - a
% procedural texture shader - is executed on the graphics processor (GPU).
% This is very fast and efficient! All parameters of the gabor patch can be
% set individually.
%
% By default - if the optional 'nonsymmetric' flag is not provided or set
% to zero - only symmetric circular gabors are drawn. If the flag is set to
% 1 then gabors with an aspect ratio ~= 1, ie., elliptical gabors, are
% drawn as well. Restricting drawing to symmetric gabors allows for an
% additional speedup in drawing (about 15% on a MacBookPro 2nd generation),
% so this may be interesting if you are in "need for speed(TM)".
%
% This demo is both, a speed benchmark, and a correctness test. If executed
% with the optional benchmark flag set to 2, it will execute a loop
% where it repeatedly draws a gabor patch both on the GPU (new style) and
% with Matlab code, then reads back and verifies the images, evaluating the
% maximum error between the old Matlab method and the new GPU method. The
% maximum error value and plotted error map are a means to assess if
% procedural shading works correctly on your setup and if the accuracy is
% sufficient for your purpose. In benchmark mode (flag set to 1), the gabor
% is drawn as fast as possible, testing the maximum rate at which your
% system can draw gabors.
%
% At a default setting of benchmark==0, it just shows nicely drawn gabor.
%
% Typical results on a MacBookPro with Radeon X1600 under OS/X 10.4.11 are:
% Accuracy: Error wrt. Matlab reference code is 0.0005536900, i.e., about
% 1 part in 2000, equivalent to a perfect display on a gfx-system with 11 bit 
% DAC resolution. Note that errors scale with spatial frequency and
% absolute magnitude, so real-world errors are usually smaller for typical
% stimuli. This is just the error, given the settings hardcoded in this script.
% Typical speed is 2800 frames per second.
%
% The error on a Radeon HD 2400 XT is 0.0000842185, i.e., about 1 part in
% 11000, equivalent to perfect display on a 13 bit DAC resolution gfx
% system.
%
% The error on a Radeon HD 5870 on OS/X 10.6 is 0.0000274425 units, about 15
% bits effective resolution, with a framerate of 8175 fps.
%
% Typical result on Intel Pentium IV, running on WindowsXP with a NVidia
% Geforce7800 and up-to-date drivers: Error is 0.0000146741 units, ie. one
% part in 68000, therefore display would be perfect even on a display device
% with 15 bit DAC's. The framerate is about 2344 frames per second.
%
% A Geforce 8800 on OS/X achieves about max error 0.0000207 units and
% a max fps of 3029 frames per second.
%
% Please note that results in performance and accuracy *will* vary,
% depending on the model of your graphics card, gfx-driver version and
% possibly operating system. For consistent results, always check before you
% measure on different setups! However, the more recent the graphics card,
% the faster and more accurate -- the latest generation of cards is
% supposed to be "just perfect" for vision research...
%
% If you want to draw many gabors per frame, you wouldn't do it like in this script,
% but use the batch-drawing version of Screen('DrawTextures', ...) instead,
% as demonstrated, e.g., in DrawingSpeedTest.m. That way you could submit
% the specs (parameters) of all gabors in one single matrix via one single
% Screen('DrawTextures'); call - this is more efficient and therefore extra
% fast!
%

% History:
% 11/26/2007 Written (MK).
% 01/03/2008 Fine tuning, help update, support for asymmetric gabors. (MK).
% 09/03/2010 Change 180+phase to 180-phase in 'DrawTextures', so phase
%            definition for Matlab reference code and GPU code match. (MK).
%            -> Reported by Xiangrui Li.

% Default to mode 0 - Just a nice demo.
if nargin < 1
    benchmark = [];
end

if isempty(benchmark)
    benchmark = 0;
end

if nargin < 2
    nonsymmetric = [];
end

if isempty(nonsymmetric)
    nonsymmetric = 0;
end

% Close previous figure plots:
close all;

% Make sure this is running on OpenGL Psychtoolbox:
AssertOpenGL;

% Initial stimulus params for the gabor patch:
res = 1*[323 323];
phase = 0;
sc = 50.0;
freq = .1;
tilt = 0;
contrast = 100.0;
aspectratio = 1.0;

% Disable synctests for this quick demo:
oldSyncLevel = Screen('Preference', 'SkipSyncTests', 2);

% Choose screen with maximum id - the secondary display:
screenid = max(Screen('Screens'));

% Setup imagingMode and window position/size depending on mode:
if benchmark==2
    rect = [0 0 res(1) res(2)];
    imagingMode = kPsychNeed32BPCFloat;
else
    rect = [];
    imagingMode = 0;
end

% Open a fullscreen onscreen window on that display, choose a background
% color of 128 = gray with 50% max intensity:
win = Screen('OpenWindow', screenid, 128, rect, [], [], [], [], imagingMode);

tw = res(1);
th = res(2);
x=tw/2;
y=th/2;

% Build a procedural gabor texture for a gabor with a support of tw x th
% pixels, and a RGB color offset of 0.5 -- a 50% gray.
gabortex = CreateProceduralGabor(win, tw, th, nonsymmetric, [0.5 0.5 0.5 0.0]);

% Draw the gabor once, just to make sure the gfx-hardware is ready for the
% benchmark run below and doesn't do one time setup work inside the
% benchmark loop: See below for explanation of parameters...
Screen('DrawTexture', win, gabortex, [], [], 90+tilt, [], [], [], [], kPsychDontDoRotation, [phase+180, freq, sc, contrast, aspectratio, 0, 0, 0]);

% Perform initial flip to gray background and sync us to the retrace:
vbl = Screen('Flip', win);
ts = vbl;
count = 0;
totmax = 0;

% Animation loop: Run for 10000 iterations:
while count < 10000
    count = count + 1;
    
    % Set new rotation angle:
    tilt = count/10;

    % Drift phase and aspectratio as well...
    if benchmark == 0
        phase = count * 10;
        aspectratio = 1 + count * 0.01;
    end
    
    % In non-benchmark mode, we also compute a gabor patch in Matlab, as a
    % reference for the optimal outcome:
    if benchmark == 2
        sf = freq;
        [gab_x gab_y] = meshgrid(0:(res(1)-1), 0:(res(2)-1));
        a=cos(deg2rad(tilt))*sf*360;
        b=sin(deg2rad(tilt))*sf*360;
        multConst=1/(sqrt(2*pi)*sc);
        x_factor=-1*(gab_x-x).^2;
        y_factor=-1*(gab_y-y).^2;
        sinWave=sin(deg2rad(a*(gab_x - x) + b*(gab_y - y)+phase));
        varScale=2*sc^2;
        m=0.5 + contrast*(multConst*exp(x_factor/varScale+y_factor/varScale).*sinWave)';
        %imshow(m);
        %drawnow;
    end
    
    % Draw the Gabor patch: We simply draw the procedural texture as any other
    % texture via 'DrawTexture', but provide the parameters for the gabor as
    % optional 'auxParameters'.
    Screen('DrawTexture', win, gabortex, [], [], 90+tilt, [], [], [], [], kPsychDontDoRotation, [180-phase, freq, sc, contrast, aspectratio, 0, 0, 0]);
    
    if benchmark > 0
        % Go as fast as you can without any sync to retrace and without
        % clearing the backbuffer -- we want to measure gabor drawing speed,
        % not how fast the display is going etc.
        Screen('Flip', win, 0, 2, 2);
    else
        % Go at normal refresh rate for good looking gabors:
        Screen('Flip', win);
    end
    
    % In non-benchmark mode, we now readback the drawn gabor from the
    % framebuffer and then compare it against the Matlab reference:
    if benchmark == 2
        % Read back, only the first color channel, but in floating point
        % precision:
        mgpu = Screen('GetImage', win, [], 'drawBuffer', 1, 1);

        %imshow(mgpu);
        %drawnow;
        % imagesc(mgpu);
        % colorbar;
        % drawnow;
        % maxval = max(max(mgpu))
        % minval = min(min(mgpu))

        % Compute per-pixel difference image of absolute differences:
        dimg = abs(mgpu - m);
        
        % Compute maximum difference value in 'totmax':
        maxdiff = max(max(dimg));
        totmax = max([maxdiff totmax]);

        % Show color-coded difference image:
        imagesc(dimg);
        colorbar;
        drawnow;
    end
    
    if benchmark~=1
        % Abort requested? Test for keypress:
        if KbCheck
            break;
        end
    end
end

% A final synced flip, so we can be sure all drawing is finished when we
% reach this point:
tend = Screen('Flip', win);

% Done. Print some fps stats:
avgfps = count / (tend - ts);
fprintf('The average framerate was %f frames per second.\n', avgfps);

% Print error measure in non-benchmark mode:
if benchmark == 2
    fprintf('The maximum difference between GPU and Matlab was %5.10f units.\n', totmax);
end

% Close window, release all ressources:
Screen('CloseAll');

% Restore old settings for sync-tests:
Screen('Preference', 'SkipSyncTests', oldSyncLevel);

% Done.
return;

function radians = deg2rad(degrees)
    radians = degrees * pi / 180;
return
