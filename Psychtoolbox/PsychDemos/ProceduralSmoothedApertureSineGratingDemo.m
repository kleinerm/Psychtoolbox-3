function ProceduralSmoothedApertureSineGratingDemo(benchmark)
% ProceduralSmoothedApertureSineGratingDemo([benchmark=0])
%
% This demo demonstrates fast drawing of smooth apertured sine wave gratings 
% using procedural shaders. It only works on hardware with support for the GLSL
% shading language, vertex- and fragment-shaders.

% History:
% 12/12/2017 modified from ProceduralGaborDemo.m (Ian Andolina).


% Default to mode 0 - Just a nice demo.
if nargin < 1
    benchmark = 0;
end

% Setup defaults and unit color range:
PsychDefaultSetup(2);

% Disable synctests for this quick demo:
oldSyncLevel = Screen('Preference', 'SkipSyncTests', 2);

% Choose screen with maximum id - the secondary display:
screenid = max(Screen('Screens'));

% Open a window 
PsychImaging('PrepareConfiguration');
win = PsychImaging('OpenWindow', screenid, 0.5);

% Enable alpha-blending
Screen('BlendFunction', win, 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA');

% Initial stimulus params for the grating:
virtualSize = 600;
phase = 0;
freq = .04;
tilt = 225;
contrast = 0.5;
% radius of the disc edge
radius = virtualSize / 2;
% smoothing sigma in pixel
sigma = 55;
% use alpha channel for smoothing?
useAlpha = true;
% smoothing method: cosine (0) or smoothstep (1)
smoothMethod = 1;


% Build a procedural gabor texture for a grating with a RGB color offset of 0.5 -- a 50% gray.
sinetex = CreateProceduralSmoothedApertureSineGrating(win, virtualSize, virtualSize,...
          [.5 .5 .5 0], radius, [], sigma, useAlpha, smoothMethod);

% Draw the grating once, just to make sure the gfx-hardware is ready for the
% benchmark run below and doesn't do one time setup work inside the
% benchmark loop:
Screen('DrawTexture', win, sinetex, [], [], tilt, [], [], [], [], [], [phase, freq, contrast, 0]);

% Perform initial flip to gray background and sync us to the retrace:
vbl = Screen('Flip', win);
tstart = vbl;
count = 0;

% Animation loop
while GetSecs < tstart + 5
    count = count + 1;
    % update values:
    phase = phase + 5;

    % Draw the grating:
    Screen('DrawTexture', win, sinetex, [], [], tilt, [], [], [], [], [], [phase, freq, contrast, 0]);

    if benchmark > 0
        % Go as fast as you can without any sync to retrace and without
        % clearing the backbuffer -- we want to measure gabor drawing speed,
        % not how fast the display is going etc.
        Screen('Flip', win, 0, 2, 2);
    else
        % Go at normal refresh rate for good looking gabors:
        Screen('Flip', win);
    end
end

% A final synced flip, so we can be sure all drawing is finished when we
% reach this point; print some stats
tend = Screen('Flip', win);
avgfps = count / (tend - tstart);
fprintf('\nPresented a total of %i frames at ~%.2g FPS...\n',count,avgfps);

% Close window, release all ressources:
sca

% Restore old settings for sync-tests:
Screen('Preference', 'SkipSyncTests', oldSyncLevel);
