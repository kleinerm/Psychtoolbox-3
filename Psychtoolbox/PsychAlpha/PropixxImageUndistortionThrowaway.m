function PropixxImageUndistortionThrowaway(calibfilename, flipmethod, corrmethod, imagefilename)
% PropixxImageUndistortionThrowaway(calibfilename [, flipmethod=0][, corrmethod=0][, imagefilename])
%
% flipmethod = 0 -> Sync flips.
% flipmethod = 1 -> Async flips for some sort of triplebuffering.
%
% corrmethod = -1 -> No correction.
% corrmethod = 0  -> Global correction for whole image.
% corrmethod = 1  -> Separate correction for each quadrant.
%

% History:
% 14-Mar-2015  mk  Initial incomplete prototype for testing.

if nargin < 2 || isempty(flipmethod)
    flipmethod = 0;
end

if nargin < 3 || isempty(corrmethod)
    corrmethod = 0;
end

% Initialize for unified KbName's and normalized 0 - 1 color range:
PsychDefaultSetup(2);

% Choose output screen as usual:
screenid=max(Screen('Screens'));

PsychImaging('PrepareConfiguration');

if flipmethod == 1
    % For drawing during async flip - aka effective triplebuffering -
    % to work, we need a virtual framebuffer:
    PsychImaging('AddTask', 'General', 'UseVirtualFramebuffer');
end

if corrmethod == 0
    PsychImaging('AddTask', 'AllViews', 'GeometryCorrection', calibfilename, 0, 73, 73, [2, 2]);
end

[w, srcSize] =PsychImaging('OpenWindow', screenid, 0);

if nargin < 4
    imagefilename = [];
end

if isempty(imagefilename)
    imagefilename = [PsychtoolboxRoot 'PsychDemos/konijntjes1024x768gray.jpg'];
end

% Use our standard bunny picture as some test case:
img = imread(imagefilename);
mytex=Screen('MakeTexture', w, img);

rate = 12

if corrmethod <= 0
    % For global correction (method 0) clear calibfilename, so local
    % correction gets disabled:
    calibfilename = [];
end
PsychProPixx('SetupFastDisplayMode', w, rate, flipmethod, calibfilename, 0);
myimg = PsychProPixx('GetImageBuffer');
Screen('TextSize', myimg, 128);

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
% benchmark loop: See below for explanation of parameters...
Screen('DrawTexture', myimg, gabortex, [], [], 90+tilt, [], [], [], [], kPsychDontDoRotation, [phase+180, freq, sc, contrast, aspectratio, 0, 0, 0]);


count = 0;
KbReleaseWait;
tstart = Screen('Flip', w, [], 2);

while ~KbCheck
  % Draw bunnies into exampleImage, scale 'em up to offscreen window size:
  if 0
    Screen('DrawTexture', myimg, mytex);
  else
    Screen('DrawTexture', myimg, gabortex, [], [], 90+count, [], [], [], [], kPsychDontDoRotation, [count+180, freq, sc, contrast, aspectratio, 0, 0, 0]);
  end

  % Some centered text for illustration...
  Screen('DrawText', myimg, sprintf('%i', count), 200, 80, 1);
  count = count + 1;

  PsychProPixx('QueueImage', myimg);
end

tend = Screen('Flip', w);
KbStrokeWait;

PsychProPixx('GetTimingSamples');

% Close all windows and ressources:
Screen('CloseAll');

avgtime = 1000 * (tend - tstart) / (count / rate)
PsychProPixx('DisableFastDisplayMode');

return;
