function [event, nremaining] = TouchEventGet(deviceIndex, windowHandle, maxWaitTimeSecs)
% [event, nremaining] = TouchEventGet(deviceIndex, windowHandle [, maxWaitTimeSecs=0])
%
% Return oldest pending event, if any, in return argument 'event', and the
% remaining number of recorded events in the event buffer of a touch
% queue in the return argument 'nremaining'.
%
% TouchEventGet() will wait up to 'maxWaitTimeSecs' seconds for at least one
% event to show up before it gives up. By default, it doesn't wait but just
% gives up if there aren't any events queued at time of invocation.
%
% 'event' is either empty if there aren't any events available, or it is a
% struct with information about the touch event. The returned event
% struct currently contains the following useful fields:
%
% 'Type' = 0 for button presses or releases if the touch device also has
%          physical or logical buttons. 'Pressed' will tell if this is a
%          button press (1) or release(0) event.
%
%        = 1 single touch point move: Only happens on some mousepads if mouse
%          emulation kicks in. 'mappedX' and 'mappedY' will tell the touch point
%          position.
%
%        = 2 New touch: 'mappedX' and 'mappedY' tell start location.
%
%        = 3 Touch moved: 'mappedX' and 'mappedY' tell new location.
%
%        = 4 Touch finished: 'mappedX' and 'mappedY' tell final location.
%
%        = 5 Touch data lost: Some other application or the GUI took over our
%            touch input, and cut us off from it, so the recorded touch data
%            sequence is incomplete. You should wait a second, then call
%            TouchEventFlush() to discard all remaining events in the queue,
%            then mark your trial invalid and take some corrective action.
%
% 'X'    = Raw touch point x-position in device and api specific coordinates. Not portable!
% 'Y'    = Raw touch point y-position in device and api specific coordinates. Not portable!
%
% 'NormX' = Normalized x-position in range 0 - 1. Constant 0 if unavailable.
% 'NormY' = Normalized x-position in range 0 - 1. Constant 0 if unavailable.
%
% 'mappedX' = x-position relative to provided onscreen window with 'windowHandle'.
% 'mappedY' = y-position relative to provided onscreen window with 'windowHandle'.
%
% 'Time' = The GetSecs time when the event was received.
%
% 'Keycode' = The unique numeric key of this touch point. A specific physical
%             touch, e.g., one specific finger touching a touchscreen, will get
%             a unique number assigned, which will persist while the finger rests
%             or moves on the touch surface, until it is lifted off the surface,
%             ie. the unique number is assigned in a Type=2 event, kept throughout
%             Type=3 events, and last used in Type=4 events, before it goes the
%             way of all mortal things. If you'd put the same finger down onto the
%             touch surface again, it would get a new unique number assigned, as
%             the touch hardware doesn't know it is the same finger.
%
% 'Pressed' = 1 while a touch point is active (finger touches screen), 0 when the
%             touch point is removed, ie. finger lifted from screen.
%
% 'Motion'  = 1 while a touch point (=finger) is moving over the surface, 0 while
%             it is resting.
%

% NOTE:
%
% Linux/X11 uses absolute touch device coordinates, which usually resolve much
% finer than display pixels, and can be mapped into absolute screen pixels, or
% window relative pixels with ease.
%
% Linux/Wayland only provides surface local coordinates, iow. the absolute pixels
% location on the is unknown, unless a fullsreen window is used where window == screen.
%
% Windows uses absolute screen coordinates in 1/100th pixels, so can be remapped
% with ease.
%
% macOS doesn't know what a touchscreen is.
%
% So X and Y is totally non-portable, but mappedX and mappedY or normX and normY
% can be computed in a reasonably meaningful way on all systems.
%
% Question: Implement mappedX and mappedY as only - onscreen window relative -
% encoding already in the mex file? We'd lose options on Windows and Linux/X11
% but would have something forward compatible?

% 1-Oct-2017  mk  Written.

if nargin < 1 || isempty(deviceIndex)
  error('Required deviceIndex missing.');
end

if nargin < 2 || isempty(windowHandle)
  error('Required windowHandle missing.');
end

if nargin < 3
  maxWaitTimeSecs = [];
end

[event, nremaining] = PsychHID('KbQueueGetEvent', deviceIndex, maxWaitTimeSecs);

if ~isempty(event)
  % Map normalized touch coordinates to window pixels:
  % XXX TODO: This is a hack only for Linux/X11 atm. for
  % basic testing. Needs more work! Do it properly in mex file?
  [w, h] = Screen('WindowSize', windowHandle);
  event.mappedX = event.NormX * w;
  event.mappedY = event.NormY * h;

  if event.mappedX == 0 && event.mappedY == 0
    event.mappedX = event.X;
    event.mappedY = event.Y;
  end

  if event.Type == 5
    fprintf('TouchEventGet: WARNING! Data loss in touch sequence detected!\n');
  end
end

return;
