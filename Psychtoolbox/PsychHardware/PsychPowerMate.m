function varargout = PsychPowerMate(cmd, varargin)
% PsychPowerMate - A driver for the Griffin Technology PowerMate USB knob.
%
% Note:
%
% This driver is currently only for the PowerMate USB edition, not for the
% bluetooth product. It also only supports modern Linux systems, e.g.,
% Ubuntu 14.04.2 LTS or later, Debian-8 or later, or other similar modern
% systems with version 2.9.0 or later of the "evdev" X11 input driver.
%
% The current driver can only address one PowerMate, as it does not have
% proper device selection code in place at the moment.
%
% Linux:
% ======
%
% You must install a specific xorg input configuration file for the PowerMate
% to be recognized. In a terminal window do:
%
% 1. cd into the Psychtoolbox working directory (output of PsychtoolboxRoot)
% 2. sudo cp PsychHardware/50-evdev-powermate.conf /usr/share/X11/xorg.conf.d/
% 3. Logout and login again.
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
% handle = PsychPowerMate('Open')
% -- Open a connected Griffin PowerMate, return a handle to it.
%
%
% PsychPowerMate('Close', handle);
% -- Close previously opened PowerMate specified by 'handle'.
%
%
% PsychPowerMate('SetBrightness', handle, level);
% -- Change brightness of the LED of the PowerMate specified by 'handle'
% to 'level'. Level can be between 0 and 255.
%
% Note: Currently 'handle' is ignored, so you can not select which one
% of multiple PowerMate's would get its brightness set and this only
% works reliably for a single PowerMate.
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
% secs = PsychPowerMate('WaitButton', handle);
% -- Wait for button press on the PowerMate specified by 'handle',
% return the GetSecs() time of press in 'secs'.
%
%
% secs = PsychPowerMate('WaitRotate', handle);
% -- Wait for knob rotation on the PowerMate specified by 'handle',
% return the GetSecs() time of rotation in 'secs'.
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

if ~IsLinux
  error('Sorry, this driver currently only supports Linux.');
end

if nargin < 1 || isempty(cmd) || ~ischar(cmd)
  help PsychPowerMate;
  return;
end

if strcmpi(cmd, 'Open')
  devId = [];
  if IsLinux
    % Enumerate all mouse devices:
    [idx, devnames, devinfos] = GetMouseIndices('slavePointer');

    % Search for 1st Griffin PowerMate:
    for i=1:length(devnames)
      if strcmp(devnames{i}, 'Griffin PowerMate')
        devId = idx(i);
        break;
      end
    end

    if isempty(devId)
      fprintf('PsychPowerMate: Could not find a Griffin PowerMate. Not plugged in or not configured? ''clear all'' might help.\n');
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

    % Create a keyboard queue for collecting button responses:
    KbQueueCreate(devId);
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

  return;
end

if strcmpi(cmd, 'SetBrightness')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if length(varargin) < 2 || isempty(varargin{2}) || ~isscalar(varargin{2}) || ~isnumeric(varargin{2}) || ~isreal(varargin{2})
    error('Brightness level missing or invalid.');
  end

  % Changing parameters is done via USB control transfers,
  % so open the USB device, then issue a control transfer,
  % then close it again.
  %
  % Currently we can't select among multiple instances of
  % the same type of device. Iow. the 'handle' is useless,
  % the control transfer will affect whatever PowerMate
  % comes first on the bus.
  %
  % The Linux event device provides a standardized way to set
  % PowerMate properties via EV_MSC, MSC_PULSELED events,
  % which would allow to select by device, but i'm too lazy
  % for that cleaner approach atm.
  usbHandle = PsychHID('OpenUSBDevice', hex2dec ('077d'), hex2dec ('0410'));
  level = min(255, max(0, varargin{2}));
  PsychHID('USBControlTransfer', usbHandle, hex2dec ('41'), 1, 1, level, 0);
  PsychHID('CloseUSBDevice', usbHandle);
  return;
end

if strcmpi(cmd, 'Get')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if IsLinux
    % Query proper "mouse" pointer device:
    [xd, yd, buttons, fd, valuator] = GetMouse([], varargin{1});
    % buttons(1) = PowerMate push button:
    varargout{1} = buttons(1);

    % valuator(3) is the dial position. valuators 1 and 2 are static dummies:
    varargout{2} = valuator(3);
  end

  return;
end

if strcmpi(cmd, 'WaitButton')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if IsLinux
    KbEventFlush(varargin{1});
    KbQueueStart(varargin{1});
    while 1
      [event, nremaining] = KbEventGet(varargin{1}, 1);
      if ~isempty(event) && event.Pressed && event.Keycode == 1
        break;
      end
    end
    KbQueueStop(varargin{1});

    % Return GetSecs time of button press:
    varargout{1} = event.Time;
  end

  return;
end

if strcmpi(cmd, 'WaitRotate')
  if length(varargin) < 1 || isempty(varargin{1})
    error('PowerMate device handle missing.');
  end

  if IsLinux
    KbEventFlush(varargin{1});
    KbQueueStart(varargin{1});
    while 1
      [event, nremaining] = KbEventGet(varargin{1}, 1);
      if ~isempty(event) && event.Pressed && event.Keycode > 1
        break;
      end
    end
    KbQueueStop(varargin{1});

    % Return GetSecs time of knob movement:
    varargout{1} = event.Time;
  end

  return;
end

error('Unknown command specified to PsychPowerMate. Read help PsychPowerMate for info.');
