function OculusVRDemo
% OculusVRDemo - A very basic demo for the Oculus VR Rift DK2

% History:
% 05-Sep-2015  mk Written.

% For debugging only:
%PsychDebugWindowConfiguration;

% Setup unified keymapping and unit color range:
PsychDefaultSetup(2);

% Select screen with highest id as Oculus output display:
screenid = max(Screen('Screens'));

% Check if at least one Oculus HMD is connected and available:
if PsychOculusVR('GetCount') > 0
  % Yes. Open and initialize connection to first detected HMD:
  fprintf('Opening the first connected Oculus VR headset.\n');
  oculus = PsychOculusVR('Open', 0);
else
  % No. Open an emulated/simulated HMD for basic testing and debugging:
  fprintf('No HMD detected. Opening a simulated HMD.\n');
  oculus = PsychOculusVR('Open', -1);
end

% Setup default rendering parameters:
PsychOculusVR('SetupRenderingParameters', oculus);

% Open our fullscreen onscreen window with black background clear color:
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'UseOculusVRHMD', oculus);
[win, rect] = PsychImaging('OpenWindow', screenid, 0, [1680, 0, 1680+1080, 1920])

for eye = 0:1
  Screen('SelectStereoDrawBuffer', win, eye);
  Screen('FillRect', win, [0 0 1]);
  Screen('FrameRect', win, [1 1 0], [], 20);
  Screen('FillOval', win, [1 1 1]);
  Screen('TextSize', win, 200);
  DrawFormattedText(win, sprintf('HELLO\nWORLD!\n%i', eye), 'center', 'center', [0 1 0]);
  %Screen('FillOval', win, [0 1 0 1]);
end

Screen('Flip', win);

KbStrokeWait;
sca;

% Close HMD, shutdown OculusVR driver:
PsychOculusVR('Close');

end
