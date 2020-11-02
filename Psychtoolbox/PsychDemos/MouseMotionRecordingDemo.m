function MouseMotionRecordingDemo
% MouseMotionRecordingDemo - Record mouse motion via KbQueues.
%
% This demo shows very basic recording of mouse/touchpad motion
% movement data under Linux and Windows.
%
% Press a key on the keyboard to end the demo.
%
% It requests recording of raw motion, ie. of the device itself,
% in device and operating system specific distance units, not
% neccessarily in screen pixels. No pointe acceleration / ballistics
% should be applied to the motion. It prints cursor position, vs.
% integrated raw position, vs. reported raw movement deltas for
% comparison. Also mouse wheel motion on some OS + device combos.
%
% Note that you may have to calibrate / map reported positions yourself
% for any given mouse device, e.g., a 400 DPI mouse may report in different
% units than a 1000 DPI mouse etc.
%
% This functionality is not supported on Apple macOS.
%

% History:
% 25-Jul-2019  mk  Written.
% 06-Oct-2020  mk  Add timestamping/dT calculation and printing.

if IsOSX
  fprintf('Sorry, this demo does not work on macOS.\n');
  return;
end

% Wait for all keyboard buttons released:
KbReleaseWait;

% Get first mouse device:
d = GetMouseIndices;
d = d(1);

% Create a keyboard queue for it, requesting return of first 3 valuators,
% ie. x and y axis, and maybe some scroll-wheel axis. Request return of
% raw motion event data (flag 4), so no pointer gain/acceleration/ballistics
% is applied by the OS and returned valuators are in device specific units
% of relative motion / movement, not absolute desktop pixel coordinates:
KbQueueCreate(d, [], 3, [], 4);

% Start movement data collection, place mouse cursor in top-left (0,0) pos:
KbQueueStart(d);
SetMouse(0,0);
[x,y] = GetMouse;
oldTime = [];
dT = 0;

fprintf('Press any key on keyboard to finish demo.\n\n');

% Repeat until keypress:
while ~KbCheck
  % Fetch all queued samples:
  while KbEventAvail(d)
    evt = KbEventGet(d);

    % Motion event? We don't care about other events:
    if evt.Type == 1
      % Accumulate absolute mouse position x,y from provided dx,dy movements:
      x = x + evt.Valuators(1);
      y = y + evt.Valuators(2);

      if ~isempty(oldTime)
        dT = evt.Time - oldTime;
      end
      oldTime = evt.Time;

      if IsWin
        % Print what we got: Desktop cursor pos (with pointer acceleration),
        % accumulated raw device position/motion, and reported increments:
        fprintf('dT=%f msecs xc=%f  yc=%f  xi=%f  yi=%f vx=%f  vy=%f  wheel %f\n', dT * 1000, evt.X, evt.Y, x, y, ...
                evt.Valuators(1), evt.Valuators(2), evt.Valuators(3));
      else
        % On Linux/X11 in raw mode, no dedicated cursor position is reported, so
        % skip that. Also, wheel position would be valuator 4, scrap that.
        fprintf('dT=%f msecs xi=%f  yi=%f vx=%f  vy=%f\n', dT * 1000, x, y, ...
                evt.Valuators(1), evt.Valuators(2));
      end
    end
  end
end

% Done. Stop data collection and clean up:
KbQueueStop(d);
KbQueueRelease(d);

fprintf('\nDone, bye!\n\n');
