function VRHMDDemo(stereoscopic, checkerboard, withGazeTracking, deviceindex)
%
% VRHMDDemo([stereoscopic=1][, checkerboard=0][, withGazeTracking=0][, deviceindex=0])
%
% A very basic demo for the most basic setup of VR HMDs, e.g., the Oculus
% VR Rift DK2. It shows the absolute minimum of steps needed - one line of
% code - to use the first connected HMD as mono or stereo display.
%
% 'stereoscopic' if set to 1 (which is the default), configures the
% HMD as a stereoscopic display. A setting of 0 configures it as a
% monoscopic display.
%
% 'checkerboard' if set to 1 draws a checkboard pattern instead of the
% regular test image.
%
% The optional parameter 'withGazeTracking', if provided and non-zero, will
% enable some basic test of eye gaze tracking with VR HMD's which support
% eye tracking. Please note that this functionality is not available in
% official Psychtoolbox releases yet, at least not as of v3.0.19.1, and the
% api used in this demo is highly experimental and subject to backwards
% incompatible changes!
%
% 'deviceindex' if provided, selects the HMD with given index. Otherwise
% the first HMD (deviceindex 0) is chosen.
%
% The demo just renders one static simple 2D image, or image pair in
% stereoscopic mode, then displays it in a loop until a key is pressed.

% History:
% 05-Sep-2015  mk Written.
% 30-Mar-2017  mk Adapt to the reality of new VR runtimes.

% Setup unified keymapping and unit color range:
PsychDefaultSetup(2);

if nargin < 1 || isempty(stereoscopic)
  stereoscopic = 1;
end

if nargin < 2 || isempty(checkerboard)
  checkerboard = 0;
end

% Disable test of eye gaze tracking by default:
if nargin < 3 || isempty(withGazeTracking)
  withGazeTracking = 0;
end

if withGazeTracking
  % Tell that eyetracking is desired:
  reqGazeTracking = 'Eyetracking ';
else
  reqGazeTracking = '';
end

if nargin < 4
  deviceindex = [];
end

% Select screen with highest id as Oculus output display:
screenid = max(Screen('Screens'));

% Open our fullscreen onscreen window with black background clear color:
PsychImaging('PrepareConfiguration');

% Build final task requirements:
basicReqs = ['NoTimingSupport NoTimestampingSupport ' reqGazeTracking];

% We do collect timestamps for benchmarking, but don't require them to be especially precise or trustworthy:
if ~stereoscopic
  % Setup the HMD to act as a regular "monoscopic" display monitor
  % by displaying the same image to both eyes:
  hmd = PsychVRHMD('AutoSetupHMD', 'Monoscopic', basicReqs, [], [], deviceindex);
else
  % Setup for stereoscopic presentation:
  hmd = PsychVRHMD('AutoSetupHMD', 'Stereoscopic', basicReqs, [], [], deviceindex);
end

if isempty(hmd)
  fprintf('No support for VR, so can not run this demo. Bye.\n');
  return;
end

[win, rect] = PsychImaging('OpenWindow', screenid);
hmdinfo = PsychVRHMD('GetInfo', hmd);

% Disable gaze tracking test if unsupported by setup:
if withGazeTracking && ~hmdinfo.eyeTrackingSupported
  withGazeTracking = 0;
end

Screen('TextStyle', win, 1);
Screen('TextSize', win, 100);

if checkerboard
  % Apply regular checkerboard pattern as texture:
  bv = zeros(32);
  wv = ones(32);
  myimg = double(repmat([bv wv; wv bv],32,32) > 0.5);
  mytex = Screen('MakeTexture', win, myimg, [], 1);
end

% Render one view for each eye in stereoscopic mode:
vbl = [];
while ~KbCheck
  for eye = 0:stereoscopic
    Screen('SelectStereoDrawBuffer', win, eye);
    Screen('FillRect', win, [0 0 1]);
    if checkerboard
      Screen('DrawTexture', win, mytex);
    end
    Screen('FrameRect', win, [1 1 0], [], 20);
    if ~checkerboard
      Screen('FillOval', win, [0.5 0.5 0.5], CenterRect([0 0 700 700], rect));
      DrawFormattedText(win, sprintf('HELLO\nWORLD!\n%i', eye), 'center', 'center', [0 1 0]);
    end
    Screen('FillOval', win, [mod(GetSecs, 1) 0 0], CenterRect([0 0 10 10], rect));

    if withGazeTracking
      if eye == 0
        state = PsychVRHMD('PrepareRender', hmd, [], 4);
      end

      Screen('FrameArc',win, [0,1,1], CenterRect([0 0 500 500], rect), mod(GetSecs, 10) * 36, 10, 20);
      for i = 1:length(state.gazeStatus)
        % fprintf('Eye %i: status %i pos = %i %i\n', i, state.gazeStatus(i), state.gazePos{i}(1), state.gazePos{i}(2));
        % disp(state.gazeRayLocal{i}.gazeC);
        if state.gazeStatus(i) >= 3
          Screen('DrawDots', win, state.gazePos{i}, 5, [1, 0, i - 1], [], 1);
        end
      end
    end
  end
  vbl(end+1) = Screen('Flip', win); %#ok<AGROW>
end

KbStrokeWait;
sca;

close all;
plot(1000 * diff(vbl));
fps = 1 / mean(diff(vbl)) %#ok<NOPRT,NASGU>

end
