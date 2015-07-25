function VideoDelayLoopMiniDemo(delayframes, duration, roi, firewireBasler)
% VideoDelayLoopMiniDemo([delayframes = 0][, duration = 30][, roi][, firewireBasler = 0]);
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
% 'firewireBasler' defaults to zero. If set to 1, customize to a Basler
% A602f IIDC-1394 compliant Firewire camera.
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
% 02/16/2010 Bugfixes and improvements. (MK)

global log

% Installation check:
AssertOpenGL;

KbName('UnifyKeyNames');

if nargin < 1 || isempty(delayframes)
    delayframes = 0; % Select zero (==minimal delay) by default.
end

if nargin < 2 || isempty(duration)
    duration = 30;
end

if nargin < 3 || isempty(roi)
    roi = [];
end

if nargin < 4 || isempty(firewireBasler)
    % Default to bog standard non IIDC-1394 camera:
    firewireBasler = 0;
end

if firewireBasler
    % For the Basler cam and other Firewire cams, use the libDC1394 capture
    % engine on OS/X instead of Apple's engine. DC1394 gives much better
    % results:
    oldEngine = Screen('Preference','DefaultVideoCaptureEngine', 1);
end

% Open onscreen window on screen:
screenid = max(Screen('Screens'));
w=Screen('OpenWindow', screenid);

PsychVideoDelayLoop('Verbosity', 10);

% Open video capture engine for delayed visual feedback. This is customized
% for the Apple builtin iSight, may need tweaking for other cams:
% The A602 Basler cam can only do grayscale, so set color flag to 0 if it
% is the Basler cam:
PsychVideoDelayLoop('Open', w , [], roi, 1 - firewireBasler);

% Try to tune and calibrate display and camera for a target framerate of 30
% Hz. This only works fully on Linux. On OS/X or Windows it'll just
% calibrate the internal timing, but not tune the display refresh interval,
% as these OS don't support such tuning:
if firewireBasler
    % Basler camera A602 can do 60 fps:
    PsychVideoDelayLoop('TuneVideoRefresh', 60);

    % Needs to close and reopen the cam after 'TuneVideoRefresh' on OS/X at
    % least, due to some bug in the Firewire stack of some versions of
    % OS/X, at least Leopard known to have trouble:
    PsychVideoDelayLoop('Close');
    PsychVideoDelayLoop('Open', w , [], roi, 1 - firewireBasler);
else
    % Bog standard cam: Assume no need to reopen the cam and only 30 fps
    % capture rate:
    PsychVideoDelayLoop('TuneVideoRefresh', 30);
end

% Set a timeout of 'duration' secs. Feedback loop runs for at most that time:
PsychVideoDelayLoop('SetAbortTimeout', duration);

% Set ESCape key as abort key to finish earlier than 'duration' seconds:
PsychVideoDelayLoop('SetAbortKeys', KbName('ESCAPE'));

% Do a fullscreen, left-right mirrored presentation:
PsychVideoDelayLoop('SetPresentation', 1, 1, 0);

% Enable timestamp logging for at most 'duration' seconds:
PsychVideoDelayLoop('SetLogging', 1, duration);

% Run the delay loop with a latency of 'delayframes' frames, allow online
% keyboard control for A602 camera:
PsychVideoDelayLoop('RunLoop', delayframes, firewireBasler);

% Done. Retrieve timing log into 'log:
log = PsychVideoDelayLoop('GetLog');

% Done. Close video capture engine and delay loop:
PsychVideoDelayLoop('Close');

% Close onscreen window, release all other ressources:
Screen('CloseAll');

if firewireBasler
    % Restore capture engine settings:
    Screen('Preference','DefaultVideoCaptureEngine', oldEngine);
end

% Plot overall latency plot for each captured/displayed frame:
close all;
plot(log(3,:) * 1000);

% Well done.
return;
