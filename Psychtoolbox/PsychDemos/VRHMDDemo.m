function VRHMDDemo(stereoscopic)
% 
% VRHMDDemo([stereoscopic=1])
%
% A very basic demo for the most basic setup of
% VR HMDs, e.g., the Oculus VR Rift DK2. It shows the
% absolute minimum of steps needed - one line of code - to
% use the first connected HMD as mono or stereo display.
%
% 'stereoscopic' if set to 1 (which is the default), configures the
% HMD as a stereoscopic display. A setting of 0 configures it as a
% monoscopic display.
%
% The demo just renders one static simple 2D image, or image
% pair in stereoscopic mode, then displays it in a loop of
% 750 repetitions to benchmark the achievable framerate.
%
% Then it waits for a keypress and then exits.
%

% History:
% 05-Sep-2015  mk Written.

% Setup unified keymapping and unit color range:
PsychDefaultSetup(2);

if nargin < 1 || isempty(stereoscopic)
  stereoscopic = 1;
end

% Select screen with highest id as Oculus output display:
screenid = max(Screen('Screens'));

% Open our fullscreen onscreen window with black background clear color:
PsychImaging('PrepareConfiguration');
if ~stereoscopic
  % Setup the HMD to act as a regular "monoscopic" display monitor
  % by displaying the same image to both eyes:
  PsychVRHMD('AutoSetupHMD', 'Monoscopic', 'LowPersistence FastResponse');
else
  % Setup for stereoscopic presentation:
  PsychVRHMD('AutoSetupHMD', 'Stereoscopic', 'LowPersistence FastResponse');
end

[win, rect] = PsychImaging('OpenWindow', screenid);

% Render one view for each eye in stereoscopic mode:
for eye = 0:stereoscopic
  Screen('SelectStereoDrawBuffer', win, eye);
  Screen('FillRect', win, [0 0 1]);
  Screen('FrameRect', win, [1 1 0], [], 20);
  Screen('FillOval', win, [1 1 1], CenterRect([0 0 900 900], rect));
  Screen('TextSize', win, 100);
  DrawFormattedText(win, sprintf('HELLO\nWORLD!\n%i', eye), 'center', 'center', [0 1 0]);
  Screen('FillOval', win, [1 0 0], CenterRect([0 0 10 10], rect));
end

% Some mini-benchmarks of performance for displaying on the HMD.
% It only measures processing time for the image post-processing
% needed, not of drawing of stimuli, as these are kept static.
tic;
for i = 1: 750
  % Do not clear framebuffer after presentation - we need to recycle it:
  Screen('Flip', win, [], 1);
end
fps = 750 / toc
KbStrokeWait;
sca;

end
