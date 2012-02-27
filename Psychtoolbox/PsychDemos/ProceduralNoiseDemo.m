function ProceduralNoiseDemo(benchmark)
% ProceduralNoiseDemo([benchmark=0])
%
% This demo demonstrates fast drawing of noise patches via use of
% procedural texture mapping. It only works on hardware with support for
% the GLSL shading language, vertex- and fragment-shaders.
%
% Patches are not encoded into a texture, but instead a little algorithm -
% a procedural texture shader - is executed on the graphics processor
% (GPU). This is very fast and efficient! All parameters of the patch can
% be set individually.
%
% This demo is both, a speed benchmark, and a correctness test. If executed
% with the optional benchmark flag set to 2, it will execute a loop where
% it repeatedly draws a patch on the GPU and then reads back the noise
% patch and computes a histogram over all pixels, so you can verify if the
% noise distribution is what you expect.
%
% In benchmark mode (flag set to 1), the patch is drawn as fast as
% possible, testing the maximum rate at which your system can draw patchs.
%
% At a default setting of benchmark==0, it just shows nicely drawn patch.
%
% Please note that results in performance and accuracy *will* vary,
% depending on the model of your graphics card, gfx-driver version and
% possibly operating system. For consistent results, always check before
% you measure on different setups! However, the more recent the graphics
% card, the faster and more accurate -- the latest generation of cards is
% supposed to be "just perfect" for vision research...
%
% If you want to draw many patches per frame, you wouldn't do it like in
% this script, but use the batch-drawing version of Screen('DrawTextures',
% ...) instead, as demonstrated, e.g., in DrawingSpeedTest.m. That way you
% could submit the specs (parameters) of all patches in one single matrix
% via one single Screen('DrawTextures'); call - this is more efficient and
% therefore extra fast!
%

% History:
% 03/18/2011 Written (MK).
% 02/26/2012 Updated (MK).

% Default to mode 0 - Just a nice demo.
if nargin < 1 || isempty(benchmark)
    benchmark = 0;
end

% Close previous figure plots:
close all;

% Make sure this is running on OpenGL Psychtoolbox:
AssertOpenGL;

% Size of the patch:
res = [512 512];

% Contrast of the noise:
contrast = 0.5;

% Disable synctests for this quick demo:
oldSyncLevel = Screen('Preference', 'SkipSyncTests', 2);

% Choose screen with maximum id - the secondary display:
screenid = min(Screen('Screens'));

% Setup imagingMode and window position/size depending on mode:
if benchmark==2
    % For histogramming, use floating point framebuffer:
    rect = [0 0 res(1) res(2)];
    imagingMode = kPsychNeed32BPCFloat;
    %imagingMode = kPsychNeed16BPCFixed; % For GPU's without 32bpc float framebuffer support.
else
    rect = [];
    imagingMode = 0;
end

% Open a fullscreen onscreen window on that display, choose a background
% color of 128 = gray with 50% max intensity:
win = Screen('OpenWindow', screenid, 128, rect, [], [], [], [], imagingMode);
tw = res(1);
th = res(2);

%[tw th] = Screen('WindowSize', win);

% Build a procedural patch texture for a patch with a support of tw x th
% pixels, and a RGB color offset of 0.5 -- a 50% gray.
%
%noisetex = CreateProceduralNoise(win, tw, th, 'Perlin', [0.5 0.5 0.5 0.0]);
noisetex = CreateProceduralNoise(win, tw, th, 'ClassicPerlin', [0.5 0.5 0.5 0.0]);

% Draw the patch once, just to make sure the gfx-hardware is ready for the
% benchmark run below and doesn't do one time setup work inside the
% benchmark loop: See below for explanation of parameters...
Screen('DrawTexture', win, noisetex, [], [], 0, [], [], [], [], [], [contrast, 0, 0, 0]);

% Perform initial flip to gray background and sync us to the retrace:
vbl = Screen('Flip', win);
ts = vbl;
count = 0;
tilt = 0;

% Animation loop: Run for 10000 iterations:
while count < 10000
    count = count + 1;

    % Set new seed value for this frame:
    seed = count;

    % Draw the patch patch: We simply draw the procedural texture as any other
    % texture via 'DrawTexture', but provide the parameters for the patch as
    % optional 'auxParameters'.
    Screen('DrawTexture', win, noisetex, [], [], tilt, [], [], [], [], [], [contrast, seed, 0, 0]);

    if benchmark > 0
        % Go as fast as you can without any sync to retrace and without
        % clearing the backbuffer -- we want to measure patch drawing speed,
        % not how fast the display is going etc.
        Screen('Flip', win, 0, 2, 2);
    else
        % Go at normal refresh rate for good looking patchs:
        Screen('Flip', win);
    end

    % In non-benchmark mode, we now readback the drawn patch from the
    % framebuffer and then compare it against the Matlab reference:
    if benchmark == 2
        % Read back, only the first color channel, but in floating point
        % precision:
        mgpu = Screen('GetImage', win, [], 'drawBuffer', 1, 1);

        % Compute and display 100 bin histogram of noise distribution:
        mgpu = mgpu(:);
        
        if 1
            hist(mgpu, 100);
            axis([0, 1, 0, tw*th/20]);
            drawnow;
        else
            autocorr = xcorr(mgpu);
            plot(autocorr);
            drawnow;
        end
        
        if count == 1
            refgpu = mgpu;
        else
            if isequal(refgpu, mgpu)
                fprintf('Repetition of 1st image detected at iteration %i.\n', count);
            end
        end
    end

    if benchmark~=1
        % Abort requested? Test for keypress:
        if KbCheck(-1)
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

% Close window, release all ressources:
Screen('CloseAll');

% Restore old settings for sync-tests:
Screen('Preference', 'SkipSyncTests', oldSyncLevel);

% Done.
return;
