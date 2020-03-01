function ProceduralColorGratingDemo(color1,color2,baseColor)
% ProceduralColorGratingDemo() -- demo color grating procedural shader
% stimuli
%
% This demo shows how to use procedural color gratings

% History:
% 01/03/2020 initial version (Ian Andolina)

if ~exist('color1','var') || isempty(color1)
	color1 = [1 0 0 1];
end

if ~exist('color2','var') || isempty(color2)
	color2 = [0 1 0 1];
end

if ~exist('baseColor','var') || isempty(baseColor)
	baseColor = [0.5 0.5 0.5 1];
end

% Setup defaults and unit color range:
PsychDefaultSetup(2);

% Disable synctests for this quick demo:
oldSyncLevel = Screen('Preference', 'SkipSyncTests', 2);

% Select screen with maximum id for output window:
screenid = max(Screen('Screens'));

% Open a fullscreen, onscreen window with gray background. Enable 32bpc
% floating point framebuffer via imaging pipeline on it, if this is possible
% on your hardware while alpha-blending is enabled. Otherwise use a 16bpc
% precision framebuffer together with alpha-blending. 
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'FloatingPoint32BitIfPossible');
[win, winRect] = PsychImaging('OpenWindow', screenid, baseColor);

% Query frame duration: We use it later on to time 'Flips' properly for an
% animation with constant framerate:
ifi = Screen('GetFlipInterval', win);

% Enable alpha-blending
Screen('BlendFunction', win, 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA');

% default x + y size
virtualSize = 512;
% radius of the disc edge
radius = floor(virtualSize / 2);
% normalise to a 2.2 gamma (normally you should linearise your monitor!)
normalise = false;

% Build a procedural texture, we also keep the shader as we will show how to
% modify it (though not as efficient as using parameters in drawtexture)
[texture, ~, shader] = CreateProceduralColorGrating(win, virtualSize,...
	virtualSize, color1, color2, radius, normalise);
			
% These settings are the parameters passed in directly to DrawTexture
% angle
angle = 45;
% phase
phase = 0;
% spatial frequency
frequency = 0.03;
% contrast
contrast = 0.5;
% sigma < 1 is a sinusoid.
sigma = -1.0;
			
% Preperatory flip
showTime = 3;
vbl = Screen('Flip', win);
tstart = vbl + ifi; %start is on the next frame

while vbl < tstart + showTime
	% Draw a message
	Screen('DrawText', win, 'Standard Color Sinusoidal Grating', 10, 10, [1 1 1]);
    % Draw the shader texture with parameters
	Screen('DrawTexture', win, texture, [], [],...
		angle, [], [], baseColor, [], [],...
		[phase, frequency, contrast, sigma]);

    Screen('DrawingFinished', win);
    
    vbl = Screen('Flip', win, vbl + 0.5 * ifi);
	phase = phase - 15;
end

%--- now we switch to a square wave grating using sigma >= 0
% if sigma is 0 then the squarewave is not smoothed, but if it is > 0 then
% hermite interpolation smoothing in +-sigma of the edge is performed.
sigma = 0.1;

% Preperatory flip
vbl = Screen('Flip', win);
tstart = vbl + ifi; %start is on the next frame

while vbl < tstart + showTime
	% Draw a message
	Screen('DrawText', win, 'Standard Color Squarewave Grating', 10, 10, [1 1 1]);
    % Draw the shader texture with parameters
	Screen('DrawTexture', win, texture, [], [],...
		angle, [], [], baseColor, [], [],...
		[phase, frequency, contrast, sigma]);

    Screen('DrawingFinished', win);
    
    vbl = Screen('Flip', win, vbl + 0.5 * ifi);
	phase = phase - 15;
end

%--- now we can change the parameters in the shader to be B-Y grating
% note this takes ~0.7ms 
glUseProgram(shader);
glUniform4f(glGetUniformLocation(shader, 'color1'), 0, 0, 1, 1);
glUniform4f(glGetUniformLocation(shader, 'color2'), 1, 1, 0, 1);
glUseProgram(0);

% Preperatory flip
vbl = Screen('Flip', win);
tstart = vbl + ifi; %start is on the next frame

while vbl < tstart + showTime
	% Draw a message
	Screen('DrawText', win, 'Blue-Yellow Color Squarewave Grating', 10, 10, [1 1 1]);
    % Draw the shader texture with parameters
	Screen('DrawTexture', win, texture, [], [],...
		angle, [], [], baseColor, [], [],...
		[phase, frequency, contrast, sigma]);

    Screen('DrawingFinished', win);
    
    vbl = Screen('Flip', win, vbl + 0.5 * ifi);
	phase = phase - 15;
end

%--- now we change the baseColor to show its effect on the grating
baseColor = [0.2 0.0 0.2 1];

% Preperatory flip
vbl = Screen('Flip', win);
tstart = vbl + ifi; %start is on the next frame

while vbl < tstart + showTime
	%draw the background with the same baseColor
	Screen('FillRect',win, baseColor);
	% Draw a message
	Screen('DrawText', win, 'Blue-Yellow Grating modifying the base color', 10, 10, [1 1 1]);
    % Draw the shader texture with parameters
	Screen('DrawTexture', win, texture, [], [],...
		angle, [], [], baseColor, [], [],...
		[phase, frequency, contrast, sigma]);

    Screen('DrawingFinished', win);
    
    vbl = Screen('Flip', win, vbl + 0.5 * ifi);
	phase = phase - 15;
end

% Close onscreen window, release all resources:
sca;

% Restore old settings for sync-tests:
Screen('Preference', 'SkipSyncTests', oldSyncLevel);
