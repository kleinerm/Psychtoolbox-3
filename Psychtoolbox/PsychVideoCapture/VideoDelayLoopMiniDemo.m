function VideoDelayLoopMiniDemo(delayframes, duration)
% VideoDelayLoopMiniDemo([delayframes = 0] [, duration = 30]);
%
% Demonstrates most simplistic use of PsychVideoDelayLoop() function for
% delayed visual feedback via a camera + display combo.
%
% 'delayframes' == Requested delay in captured frames: 0 = Minimal delay
% (i.e. request zero delay, get whatever the minimum of your camera +
% display combo is).
%
% The demo takes some startup time to calibrate, then provides visual
% feedback for 30 seconds, then ends, unless you specifiy a different
% 'duration' in seconds.
%
% Tested on MacOS/X Intel MacBookPro with builtin iSight camera. However,
% the routine was originally developed, tested and optimized for GNU/Linux,
% so it only provides limited functionality, flexibility and performance on
% other operating systems. The inherent delay of USB cameras like Apples
% builtin iSight is pretty high, so they are not seriously useful for low
% delay feedback. Pick a good IIDC Firewire camera if you want low
% latencies. Use Linux if you want really low latencies and optimal
% control.
%
% This demo is only a proof of concept. Use with caution!

% History:
% 06/09/2007 Written (MK)

% Installation check:
AssertOpenGL;

if nargin < 1 || isempty(delayframes)
    delayframes = 0; % Select zero (==minimal delay) by default.
end

if nargin < 2 || isempty(duration)
    duration = 30;
end

% Open onscreen window on screen:
screenid = max(Screen('Screens'));
w=Screen('OpenWindow', screenid);

% Open video capture engine for delayed visual feedback. This is customized
% for the Apple builtin iSight, may need tweaking for other cams:
handle = PsychVideoDelayLoop('Open', w , [], [], 1);

% Set a timeout of 10 secs. Feedback loop runs for at most 10 seconds:
PsychVideoDelayLoop('SetAbortTimeout', duration);

% Try to tune and calibrate display and camera for a target framerate of 30
% Hz. This only works fully on Linux. On OS/X or Windows it'll just
% calibrate the internal timing, but not tune the display refresh interval,
% as these OS don't support such tuning:
PsychVideoDelayLoop('TuneVideoRefresh', 30);

% Run the delay loop with a latency of 'delayframes' frames:
PsychVideoDelayLoop('RunLoop', delayframes);

% Done. Close video capture engine and delay loop:
PsychVideoDelayLoop('Close');

% Close onscreen window, release all other ressources:
Screen('CloseAll');

% Well done.
return;
