function PropixxImageUndistortionThrowaway(calibfilename, flipmethod, corrmethod, imagefilename)
% PropixxImageUndistortionThrowaway(calibfilename [, flipmethod=0][, corrmethod=0][, imagefilename])
%
% A preliminary throwaway demo of how to use PropPixx in fast 4x or 12x
% display mode. This demo is EXPERIMENTAL and subject to change
% without warning, potentially even removal, in future Psychtoolbox
% releases. Make a private copy of it and the PsychProPixx.m file
% if you want to keep it as is for production use!
%
% You will need a powerful, fast graphics card and a well configured
% operating system and machine for this to work reliably at 12x mode.
%
% The demo can be stopped via the ESCAPE key.
%
% Parameters and their meaning:
%
% 'calibfilename' mandatory name of a display geometry calibration file
% created via DisplayUndistortionBVL() iff corrmethod is >= 0.
%
% flipmethod = 0 -> Sync flips. [ Slow but easy ]
% flipmethod = 1 -> Async flips for some sort of triplebuffering. [ Potentially faster but more difficult ]
% flipmethod = 2 -> Fastest method, but only available on Linux with open-source graphics drivers.
%
% corrmethod = -1 -> No correction.
% corrmethod = 0  -> Global correction for whole image. [Fastest and default]
% corrmethod = 1  -> Separate correction for each quadrant.
%
% 'imagefilename' optional name of image file to display - see code.

% History:
% 14-Mar-2015  mk  Initial incomplete prototype for testing.

if nargin < 2 || isempty(flipmethod)
    % flipmethod 0 is the most safe, but also the least
    % efficient:
    flipmethod = 0;
end

if nargin < 3 || isempty(corrmethod)
    % corrmethod 0 is the most efficient one:
    corrmethod = 0;
end

% Initialize for unified KbName's and normalized 0 - 1 color range:
PsychDefaultSetup(2);

% Only check ESCAPE key via KbCheck for ending the demo.
% KbCheck is expensive especially on OSX, so minimize its
% impact to the absolute minimum:
RestrictKeysForKbCheck(KbName('ESCAPE'));

% Choose output screen as usual:
screenid = max(Screen('Screens'));

PsychImaging('PrepareConfiguration');

if flipmethod == 1
    % For drawing during async flip - aka effective triplebuffering -
    % to work, we need a virtual framebuffer:
    PsychImaging('AddTask', 'General', 'UseVirtualFramebuffer');
end

if corrmethod == 0
    % Use global geometry correction, with identical correction
    % applied to all 4 quadrants of the onscreen window, ie., use
    % a 2 by 2 tiling [2,2]. This is currently only implemented for
    % the DisplayUndistortionBVL method. Subdivide warp mesh into
    % a 73 x 73 grid. This may be much finer than needed and lowering
    % the number may increase performance at a loss of precision.
    PsychImaging('AddTask', 'AllViews', 'GeometryCorrection', calibfilename, 0, 73, 73, [2, 2]);
end

w = PsychImaging('OpenWindow', screenid, 0);

if nargin < 4
    imagefilename = [];
end

if isempty(imagefilename)
    imagefilename = [PsychtoolboxRoot 'PsychDemos/konijntjes1024x768gray.jpg'];
end

% Use our standard bunny picture as some test case:
img = imread(imagefilename);
mytex=Screen('MakeTexture', w, img);

% Try to present at 12x rate ie. 1440 Hz stimulus update rate:
rate = 12

if corrmethod <= 0
    % For global correction (method 0) clear calibfilename, so local
    % correction gets disabled: Global correction is faster on all
    % tested systems.
    calibfilename = [];
end

% Setup for fast display mode, producing the final image in onscreen window
% 'w', for presentation at rate 'rate' (4 or 12), with 'flipmethod'.
% Replace 0 with 1 for GPU load benchmarking - has some performance impact
% itself, but allows assessment of how much we make the graphics card sweat:
PsychProPixx('SetupFastDisplayMode', w, rate, flipmethod, calibfilename, 0);

% Get a suitable offscreen window 'myimg' for drawing our stimulus:
myimg = PsychProPixx('GetImageBuffer');

% We want text to be big:
Screen('TextSize', myimg, 128);

% Setup for procedural gabor - stolen from ProceduralGaborDemo:
res = [500 500];
phase = 0;
sc = 50.0;
freq = .1;
tilt = 0;
contrast = 100.0;
aspectratio = 1.0;
tw = res(1);
th = res(2);

% Build a procedural gabor texture for a gabor with a support of tw x th
% pixels, and a RGB color offset of 0.5 -- a 50% gray.
gabortex = CreateProceduralGabor(myimg, tw, th, 0, [0.5 0.5 0.5 0.0]);

% Draw the gabor once, just to make sure the gfx-hardware is ready for the
% benchmark run below and doesn't do one time setup work inside the
% benchmark loop:
Screen('DrawTexture', myimg, gabortex, [], [], 90+tilt, [], [], [], [], kPsychDontDoRotation, [phase+180, freq, sc, contrast, aspectratio, 0, 0, 0]);

% Realtime scheduling:
Priority(MaxPriority(w));

% Prepare the benchmark / demo run:
count = 0;
KbReleaseWait;
tstart = Screen('Flip', w);

while 1
  if 0
    % Draw bunnies into exampleImage:
    Screen('DrawTexture', myimg, mytex);
  else
    % Use a gabor shader for fast procedural drawing of Gabor stim:
    Screen('DrawTexture', myimg, gabortex, [], [], 90+count, [], [], [], [], kPsychDontDoRotation, [count+180, freq, sc, contrast, aspectratio, 0, 0, 0]);
  end

  % Some centered text for illustration... Text drawing is
  % relatively expensive, especially so on OSX and Windows!
  Screen('DrawText', myimg, sprintf('%i', count), 200, 80, 1);
  count = count + 1;

  % Queue this stimulus for presentation at soonest video
  % refresh cycle of the graphics card.
  % Call KbCheck only on QueueImage calls which actually
  % trigger a stimulus image update, ie. every 4th or 12th
  % call. KbCheck calls are relatively expensive and can
  % really spoil the timing on excessive use:
  if PsychProPixx('QueueImage', myimg) && KbCheck
    % ESCape key pressed, end this demo:
    break;
  end
end

% Some magic to keep some weird driver happy on some OS:
WaitSecs(0.1);
Screen('FillRect', w, 0);

% Final flip for time measurement:
tend = Screen('Flip', w);

% Done with realtime:
Priority(0);

% Retrieve timing samples, mostly useful for flipmethod 2:
PsychProPixx('GetTimingSamples');

% Reenable KbCheck for all keys:
RestrictKeysForKbCheck([]);

% Close all windows and ressources:
Screen('CloseAll');

avgframetimemsecs = 1000 * (tend - tstart) / (count / rate)

% Disable driver, plot some timing plots:
PsychProPixx('DisableFastDisplayMode', 1);

return;
