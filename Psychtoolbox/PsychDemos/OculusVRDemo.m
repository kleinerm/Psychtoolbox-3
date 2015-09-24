function OculusVRDemo
% OculusVRDemo - A very basic demo for the Oculus VR Rift DK2

% History:
% 05-Sep-2015  mk Written.

% Setup unified keymapping and unit color range:
PsychDefaultSetup(2);

% Select screen with highest id as Oculus output display:
screenid = max(Screen('Screens'));

% Open our fullscreen onscreen window with black background clear color:
PsychImaging('PrepareConfiguration');
PsychVRHMD('AutoSetupHMD');
[win, rect] = PsychImaging('OpenWindow', screenid, 0) %, [1680, 0, 1680+1080, 1920])

for eye = 0:1
  Screen('SelectStereoDrawBuffer', win, eye);
  Screen('FillRect', win, [0 0 1]);
  Screen('FrameRect', win, [1 1 0], [], 20);
  Screen('FillOval', win, [1 1 1], CenterRect([0 0 900 900], rect));
  Screen('TextSize', win, 200);
  DrawFormattedText(win, sprintf('HELLO\nWORLD!\n%i', eye), 'center', 'center', [0 1 0]);
  Screen('FillOval', win, [1 0 0], CenterRect([0 0 10 10], rect));
end

tic;
for i = 1: 750
Screen('Flip', win, [], 1);
end
fps = 750 / toc

global img;
img = Screen('GetImage', win);

KbStrokeWait;
sca;
end
