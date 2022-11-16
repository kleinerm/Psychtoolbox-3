function varargout = PsychPowerMate(cmd, varargin)
% PsychPowerMate - A driver for the Griffin Technology PowerMate USB knob.
%
% Note:
%
% This driver is currently only for the PowerMate USB edition, not for the
% bluetooth product. It best supports modern Linux systems, e.g.,
% Ubuntu 14.04.2 LTS or later, Debian-8 or later, or other similar modern
% systems with version 2.9.0 or later of the "evdev" X11 input driver.
%
% Support on Apple OSX and Microsoft Windows is much more limited. Expect
% worse reliability, higher cpu load, higher latencies, less accurate
% timing. While on Linux, the current driver can internally record up to
% 104 full knob rotations by default, or an unlimited number with suitable
% options set, without losing information, the Windows variant can at most
% record 5 rotations, and the OSX driver will probably not even manage to
% record one full rotation. To avoid loss of position data, you will need
% to call PsychPowerMate('Get') frequently on OSX and Windows, even if you
% do not need the information at that moment.
%
% On Windows, setting LED brightness does not work.
%
% Linux setup:
% ============
%
% You must install a specific xorg input configuration file for the PowerMate
% to be recognized. In a terminal window do:
%
% 1. cd into the Psychtoolbox working directory (output of PsychtoolboxRoot)
% 2. sudo cp PsychHardware/50-evdev-powermate.conf /usr/share/X11/xorg.conf.d/
% 3. Logout and login again.
%
% Interesting technical background info:
% ======================================
%
% Technical background info, copied from the Linux powermate.c device driver,
% written by William R Sowerbutts, the author of the Linux driver:
%
% "Testing with the knob I have has shown that it measures approximately 94 "clicks"
% for one full rotation. Testing with my High Speed Rotation Actuator (ok, it was
% a variable speed cordless electric drill) has shown that the device can measure
% speeds of up to 7 clicks either clockwise or anticlockwise between pollings from
% the host. If it counts more than 7 clicks before it is polled, it will wrap back
% to zero and start counting again. This was at quite high speed, however, almost
% certainly faster than the human hand could turn it. Griffin say that it loses a
% pulse or two on a direction change; the granularity is so fine that I never
% noticed this in practice."
%
% - The rotary resolution is therefore about 3.8298 degrees per "click" or unit.
%
% - The PowerMate can lose a click or two on a direction change, therefore can
%   accumulate rotational error on each turn direction change.
%
% - The PowerMate wants to be sampled at 10 msec intervals according to its
%   USB HID descriptor. Low-speed USB interrupt endpoint sampling interval
%   duration must be a power of two, so it will actually get sampled at
%   8 msec intervals, for a maximum update rate of 125 Hz.
%
%
% Commands:
%
% deviceIds = PsychPowerMate('List');
% -- Return a list of all 'deviceIds' of all connected PowerMates. Items from
% this list can be passed as 'deviceId' parameter to the 'Open' function to
% select a specific PowerMate. Please note that the PowerMates themselves do
% not provide any identifying information, like a serial number, so the computer
% can't distinguish them. The 'deviceIds' you get back from this function are
% therefore only guaranteed to be valid during a given session. Unplugging and
% replugging PowerMates may change the ids assigned to them, based on operating
% system specific criteria. On MS-Windows the deviceIds don't change even
% if you plug the PowerMate into a different USB port, so this function may
% be useless for device selection and you may have to figure out something
% specific to your computer hardware setup. On Linux you can assign fixed
% ids to fixed USB ports. See the Technical note at the end of this help
% text for how to do it. On OSX the deviceIds seem to be associated with
% fixed USB ports, with numbers at least persistent during a session.
%
%
% handle = PsychPowerMate('Open' [, deviceId][, historySize=10000])
% -- Open a connected Griffin PowerMate, return a handle to it.
%
% The optional 'deviceId' selects which of multiple connected PowerMates should
% be used. PsychPowerMate('List') returns a list of all possible 'deviceId's.
% If 'deviceId' is omitted then the first found PowerMate is opened.
%
% Linux only:
%
% The optional 'historySize' parameter allows to select how many events the
% knob press and motion history functions 'StartHistory' and 'GetHistory' can
% collect in the background. It defaults to 10000 events if omitted, ie. up to
% 5000 knob presses + releases, or alternatively about 104 full knob rotations
% can be recorded in the background before you either need to get the recorded
% data via the 'GetHistory' subfunction to empty the internal storage, or the
% internal storage will be full and stop collecting new data.
%
% If you need to record more than 10000 such events in total, just specify a
% larger 'historySize': Each button press or release creates 2 events (1 press +
% 1 release). Each turn of the knob by 1 unit (about 3.8 degrees) creates one
% event.
%
%
% PsychPowerMate('Close', handle);
% -- Close previously opened PowerMate specified by 'handle'.
%
%
% PsychPowerMate('SetBrightness', handle, level);
% -- Change brightness of the LED of the PowerMate specified by 'handle'
% to 'level'. Level can be between 0 and 255. 'handle' is currently ignored
% on Linux.
%
%
% [button, dialPos] = PsychPowerMate('Get', handle);
% -- Poll the PowerMate specified by 'handle', return the status
% of its 'button' 1 = Pressed, 0 = Released. Also return the current
% 'dialPos' dial position of its dial - its turning knob. Please note
% that the knob does not have a defined zero position. You can turn it
% endlessly in one direction. Turning in one direction will simply
% increment the 'dialPos' position, turning into the other direction
% will decrement it. See above comments on the accuracy and resolution
% of position reporting.
%
%
% secs = PsychPowerMate('WaitButton', handle [, waitforbuttonpress=1][, untilTime=inf]);
% -- Wait for button press or release on the PowerMate specified by 'handle',
% return the GetSecs() time of press in 'secs'.
%
% If the optional 'waitforbuttonpress' flag is set to 0 then the function
% waits for a button release, otherwise it waits for a button press.
%
% If the optional 'untilTime' parameter is given, the function times out
% at time 'untilTime', otherwise it waits forever if nobody presses or releases
% the button.
%
% secs = PsychPowerMate('WaitRotate', handle [, untilTime=inf]);
% -- Wait for knob rotation on the PowerMate specified by 'handle',
% return the GetSecs() time of rotation in 'secs'.
%
% If the optional 'untilTime' parameter is given, the function times out
% at time 'untilTime', otherwise it waits forever if nobody moves the knob.
%
%
% The following functions are only supported on Linux:
% ----------------------------------------------------
%
% PsychPowerMate('StartHistory', handle [, clearHistory=0]);
% -- Start recording of a knob press and knob motion history in the background.
% If the optional 'clearHistory' flag is set to 1 then the current history is
% cleared/discarded before start of recording of a new one.
%
%
% PsychPowerMate('StopHistory', handle);
% -- Stop recording of knob press and knob motion history.
% You can resume recording via 'StartHistory' or start recording a new one,
% discarding old data via 'StartHistory' with the flag clearHistory=1 set.
%
%
% history = PsychPowerMate('GetHistory', handle);
% -- Retrieve currently recorded knob press and knob motion history.
% history is a 4-by-n matrix with n columns for n recorded events.
% Row 1 contains knob position. Row 2 contains knob button state: 0 = Released,
% 1 = Pressed. Row 3 contains a GetSecs timestamp of the element. Row 4 tells
% if the given column records a knob press/release (0) or a knob movement (1).
%
%
% numItems = PsychPowerMate('GetHistorySize', handle);
% -- Return number of items currently recorded in history.
%
%
%
% Technical note: How to map physical USB bus location to a /dev/input/powermateXX
% symlink via a udev rule, then how to map that device file to a xinput device
% name for enumeration/selection by our driver. Follow the approach outlined
% at the bottom of this thread for dual-touchscreen setups with identical touchscreen:
%
% http://arstechnica.com/civis/viewtopic.php?f=16&t=1146846
%

% History:
% 08-Apr-2016  mk  Written.
% 06-Oct-2017  mk  Add release flag to 'WaitButton' to wait for button release.
%                  Add 'untilTime' to 'WaitButton' and 'WaitRotate' to allow a
%                  wait with timeout.
%                  Simplify WaitXXX functions to use same code pathes on all OS,
%                  ie. drop Linux and OSX special cases. This simplifies the code,
%                  and should have no practical impact on precision or performance.
%                  Add ability to select one of multiple PowerMates by 'deviceId'.
%                  New 'List' function lists 'deviceId's of all connected PowerMates.
%
%                  New functions for Linux only for recording button activity and
%                  knob motion in the background via our improved keyboard queues,
%                  which can now track valuator changes and values, and thereby knob
%                  movement and positions.
% 25-Oct-2022  mk  Add check and warning if USB control transfers unsupported on macOS.
% 15-Nov-2022  mk  Reimplement 'SetBrightness' using PsychHID('SetReport'). Now works on all systems.

persistent turnval;
persistent buttonval;
persistent firstcall;
persistent lastDialPos;

if ~IsLinux && isempty(firstcall)
  firstcall = 0;
  warning('This driver only works perfectly on Linux. Read help carefully for use on this legacy operating system.');
end

if nargin < 1 || isempty(cmd) || ~ischar(cmd)
  help PsychPowerMate;
  return;
end

if strcmpi(cmd, 'List')
  if IsLinux
    [idx, devnames, devinfos] = GetMouseIndices('slavePointer', 'Griffin PowerMate');
    if ~isempty(idx)
      varargout{1} = devinfos{:}.interfaceID;
    else
      varargout{1} = [];
    end
  else
    LoadPsychHID;
    devinfos = PsychHID('Devices');
    devId = [];
    for i=1:length(devinfos)
      if (devinfos(i).vendorID == 1917) && (devinfos(i).productID == 1040)
        devId(end+1) = devinfos(i).locationID; %#ok<AGROW>
      end
    end
    varargout{1} = devId;
  end

  return;
end

if strcmpi(cmd, 'Open')
  devId = [];

  % Device id provided?
  if (length(varargin) >= 1) && ~isempty(varargin{1})
    deviceId = varargin{1};
    if ~isnumeric(deviceId) || ~isscalar(deviceId)
      error('Invalid deviceId. Provided deviceId must be a single number.');
    end
  else
    deviceId = [];
  end

  % historySize provided?
  if (length(varargin) >= 2) && ~isempty(varargin{2})
    historySize = varargin{2};
    if ~isnumeric(historySize) || ~isscalar(historySize) ||  (historySize < 1)
      error('Invalid historySize. Provided historySize must be a single number of at least 1.');
    end
  else
    historySize = [];
  end

  if IsLinux
    % Enumerate all mouse devices:
    [idx, devnames, devinfos] = GetMouseIndices('slavePointer');

    % Search for 1st Griffin PowerMate or specified one via 'deviceId':
    for i=1:length(devnames)
      if strcmp(devnames{i}, 'Griffin PowerMate') && ...
         (isempty(deviceId) || (devinfos{i}.interfaceID == deviceId))
        devId = idx(i);
        break;
      end
    end

    if isempty(devId)
      if isempty(deviceId)
        fprintf('PsychPowerMate: Could not find a Griffin PowerMate. Not plugged in or not configured? ''clear all'' might help.\n');
      else
        fprintf('PsychPowerMate: Could not find a Griffin PowerMate with specified deviceId %i. Not plugged in or not configured? ''clear all'' might help.\n', deviceId);
      end

      varargout{1} = [];
      varargout{2} = [];
      return;
    end

    % Return "mouse" device index:
    varargout{1} = devId;

    % Return a somewhat identifying location id. interfaceID is the
    % Xinput id of the device, the best we have for differentiating
    % between multiple PowerMate's.
    varargout{2} = devinfos{i}.interfaceID;

    % Create a keyboard queue for collecting button responses and knob motions:
    KbQueueCreate(devId, [], 3, historySize, 1);
  end

  if IsOSX || IsWin
    LoadPsychHID;
    devinfos = PsychHID('Devices');

    % Search for 1st Griffin PowerMate or specified one via 'deviceId':
    for i=1:length(devinfos)
      if (devinfos(i).vendorID == 1917) && (devinfos(i).productID == 1040) && ...
         (isempty(deviceId) || (devinfos(i).locationID == deviceId))
        devId = devinfos(i).index;
        break;
      end
    end

    if isempty(devId)
      if isempty(deviceId)
        fprintf('PsychPowerMate: Could not find a Griffin PowerMate. Not plugged in or not configured? ''clear all'' might help.\n');
      else
        fprintf('PsychPowerMate: Could not find a Griffin PowerMate with specified deviceId %i. Not plugged in or not configured? ''clear all'' might help.\n', deviceId);
      end

      varargout{1} = [];
      varargout{2} = [];
      return;
    end

    % Return "mouse" device index:
    varargout{1} = devId;

    % Return a somewhat identifying location id. locationID is the
    % best we have for differentiating between multiple PowerMate's.
    varargout{2} = devinfos(i).locationID;

    % Start HID report reception from PowerMate, with 1 msecs max
    % processing time allocated per pass:
    options.secs = 0.001;
    PsychHID('ReceiveReports', devId, options);

    % Init variables for tracking knob state:
    turnval(devId) = 0;
    buttonval(devId) = logical(0);
  end

  return;
end

if strcmpi(cmd, 'Close')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if IsLinux
    KbQueueStop(varargin{1});
    KbQueueRelease(varargin{1});
  end

  if IsOSX || IsWin
    PsychHID('ReceiveReportsStop', varargin{1});
    PsychHID('GiveMeReports', varargin{1});
  end

  return;
end

if strcmpi(cmd, 'SetBrightness')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if length(varargin) < 2 || isempty(varargin{2}) || ~isscalar(varargin{2}) || ~isnumeric(varargin{2}) || ~isreal(varargin{2})
    error('Brightness level missing or invalid.');
  end

  level = min(255, max(0, varargin{2}));

  if IsLinux
    % Linux: Do it old school, because the passed in handle is not a HID device
    % handle, but a handle XINPUT devices or similar:
    usbHandle = PsychHID('OpenUSBDevice', hex2dec ('077d'), hex2dec ('0410'));
    PsychHID('USBControlTransfer', usbHandle, hex2dec ('41'), 1, 1, level, 0);
    PsychHID('CloseUSBDevice', usbHandle);
  else
    % Windows, macOS: HID out report instead of USB control transfers, because it
    % respects the device handle and works cross-platform:
    PsychHID('SetReport', varargin{1}, 2, 0, uint8(level));
  end

  return;
end

if strcmpi(cmd, 'Get')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  % Linux keeps track of live button state and turn angle, can just query:
  if IsLinux
    % Query proper "mouse" pointer device:
    [~, ~, buttons, ~, valuator] = GetMouse([], varargin{1});

    % buttons(1) = PowerMate push button:
    varargout{1} = buttons(1);

    % valuator(3) is the dial position. valuators 1 and 2 are static dummies:
    varargout{2} = valuator(3);

    return;
  end

  % OSX and Windows can not keep track of live state. Must derive button
  % state and knob turn angle from accumulated deltas in HID reports:
  if IsOSX || IsWin
    accumlength = 0;

    while 1
      PsychHID('ReceiveReports', varargin{1});
      reports = PsychHID('GiveMeReports', varargin{1});
      if isempty(reports)
        break;
      end

      accumlength = accumlength + length(reports);

      for i=1:length(reports)
        turnval(varargin{1}) = turnval(varargin{1}) + double(typecast(reports(i).report(2), 'int8'));
        buttonval(varargin{1}) = logical(reports(i).report(1));
      end
    end

    % MS-Windows can keep track of at most 512 reports aka PowerMate "clicks",
    % if more reports arrive inbetween calls to 'Get', we will lose them and
    % therefore have wrong knob position reporting from now on:
    if accumlength >= 512 && IsWin
      fprintf('PsychPowerMate: Processed %d samples, more than system limit of 512 samples!\n', accumlength);
      warning('PsychPowerMate(''Get''): Input samples from PowerMate may have been lost, position reporting might be wrong!');
    end

    varargout{1} = buttonval(varargin{1});
    varargout{2} = turnval(varargin{1});
  end

  return;
end

if strcmpi(cmd, 'WaitButton')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if length(varargin) < 2 || isempty(varargin{2})
    pressed = 1;
  else
    pressed = varargin{2};
  end

  if length(varargin) < 3 || isempty(varargin{3})
    untilTime = inf;
  else
    untilTime = varargin{3};
  end

  PsychPowerMate('Get', varargin{1});
  while (PsychPowerMate('Get', varargin{1}) ~= pressed) && (GetSecs < untilTime)
    % On Linux and OSX 'YieldSecs' is as accurate as regular WaitSecs:
    WaitSecs('YieldSecs', 0.001);
  end

  % Return GetSecs time of button press:
  varargout{1} = GetSecs;

  return;
end

if strcmpi(cmd, 'WaitRotate')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if length(varargin) < 2 || isempty(varargin{2})
    untilTime = inf;
  else
    untilTime = varargin{2};
  end

  [dummy, baseRotate] = PsychPowerMate('Get', varargin{1});
  while GetSecs < untilTime
    [dummy, curRotate] = PsychPowerMate('Get', varargin{1});
    if curRotate ~= baseRotate
      break;
    end

    % On Linux and OSX this is as accurate as a regular WaitSecs:
    WaitSecs('YieldSecs', 0.001);
  end

  % Return GetSecs time of button press:
  varargout{1} = GetSecs;

  return;
end

if strcmpi(cmd, 'GetHistory')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if IsLinux
    trace = [];
    while 1
      event = KbEventGet(varargin{1});
      if isempty(event)
        break;
      end

      switch event.Type
        case 0,
          % Button press/release. We only care for the real PowerMate button:
          if event.Keycode == 1
            % Doesn't contain current knob position, so get it from lastDialPos
            % tracking.
            trace(1, end+1) = lastDialPos(varargin{1});
            trace(2, end)   = event.Pressed;
            trace(3, end)   = event.Time;
            trace(4, end)   = 0; % Type is 0 = Button/Press release.
          end

        case 1,
          % Motion/Valuator change: Records knob motion, but also contains
          % current button state:
          lastDialPos(varargin{1}) = event.Valuators(3);
          trace(1, end+1) = event.Valuators(3);
          trace(2, end)   = event.Pressed;
          trace(3, end)   = event.Time;
          trace(4, end)   = 1; % Type is 1 = Knob motion.
      end
    end

    % Return trace of knob movement and button state:
    varargout{1} = trace;

    return;
  end

  if IsOSX || IsWin
    error('Recording motion and knob state traces is only supported on Linux.');
  end

  return;
end

if strcmpi(cmd, 'StartHistory')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if IsLinux
    if length(varargin) >= 2 && ~isempty(varargin{2}) && varargin{2} == 1
      % Flush history buffer before start:
      KbEventFlush(varargin{1});
    end

    % Init last dial position with current position:
    [~, ~, ~, ~, valuator] = GetMouse([], varargin{1});
    lastDialPos(varargin{1}) = valuator(3);

    % Start recording:
    KbQueueStart(varargin{1});
  end

  if IsOSX || IsWin
    error('Recording motion and knob state traces is only supported on Linux.');
  end

  return;
end

if strcmpi(cmd, 'StopHistory')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if IsLinux
    % Stop recording:
    KbQueueStop(varargin{1});
  end

  if IsOSX || IsWin
    error('Recording motion and knob state traces is only supported on Linux.');
  end

  return;
end

if strcmpi(cmd, 'GetHistorySize')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if IsLinux
    varargout{1} = KbEventAvail(varargin{1});
  end

  if IsOSX || IsWin
    error('Recording motion and knob state traces is only supported on Linux.');
  end

  return;
end

error('Unknown command specified to PsychPowerMate. Read help PsychPowerMate for info.');
