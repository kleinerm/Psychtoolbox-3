function secs = KbTriggerWait(keyCode, deviceNumber)
% secs = KbTriggerWait(keyCode, [deviceNumber])
%
% Waits until trigger has been pressed and returns the time in seconds.
% The keyCode argument should be a single number. For example, to check
% for the 't' key as the trigger use KbTriggerWait(KbName('t'))
%
% You cannot use KbTriggerWait while a queue created by KbQueueCreate
% exists. To shut down such a queue, use KbQueueRelease.
%
% On platforms other than Mac OS X 10.3 and later, this function simply
% serves as a convenient substitute for using KbCheck to detect the
% trigger of interest.
%
% OSX 10.3 and later: ____________________________________________________
%
% This function will allow triggers to be reliably detected from devices
% that only briefly report that the key is down. KbCheck is not reliable
% with such devices because it may not poll often enough to detect the
% key down state. This function will not address this problem on other
% platforms or with earlier Macintosh operating system versions.
%
% KbTriggerWait uses the PsychHID function, a general purpose function for
% reading from the Human Interface Device (HID) class of USB devices.
% Unlike KbCheck, it starts a queue that receives keyboard events
% regarding the trigger key (and no other keys) and then polls this queue
% (rather than the current key status) periodically. In theory, this
% should also provide more accurate reporting of the time of the
% triggering keypress. However, if multiple trigger events have occurred
% since last polled, it is possible that the timestamp of the earliest
% of these will have already rotated out of the limited capacity (eight
% events) queue. In this case, the time of the earliest event remaining
% in the queue is reported. Since the polling frequency is the same as
% KbCheck, it should be more accurate on average with regard to timing,
% even when the timestamps of the earliest events have been lost due to
% queue overflow.
%
% KbTriggerWait tests the first USB-HID keyboard device by default.
% Optionally, you can pass in a 'deviceNumber' to test a different keyboard
% if multiple keyboards are connected to your machine.
%
% Passing a deviceNumber of -1 will NOT cause all keyboards to be detected
%
% One disadvantage of this function is that it renders Matlab relatively
% unresponsive to Ctrl-C interrupts. KbQueueWait is a better option in
% this regard, but more complicated to use.
% _________________________________________________________________________
%
% See also: KbQueueWait KbCheck, KbWait, GetChar, CharAvail, KbDemo.

% 8/10/07    rpw  Wrote it.
% 8/21/07	 rpw  Added comments about KbQueueWait as alternative
% 8/23/07    rpw  Added warning about incompatibility with KbQueueCreate, et al.

% Unless we are running Mac OS X 10.3 or later, default to KbCheck based
% method. We sort this out on the first call and then store the result
% in macosrecent for subsequent calls

persistent macosxrecent;
if isempty(macosxrecent)
    macosxrecent = IsOSX;
end

if macosxrecent | IsLinux
    if nargin==2
        [secs]= PsychHID('KbTriggerWait', keyCode, deviceNumber);
    elseif nargin == 1
        [secs]= PsychHID('KbTriggerWait', keyCode);
    elseif nargin == 0
        error('Trigger key code must be specified in KbTriggerWait');
    elseif nargin > 2
        error('Too many arguments supplied to KbTriggerWait');
    end
else
    % We use the built-in KbCheck facility of Screen on GNU/Linux and MS-Windows
    % for KbChecks until a PsychHID implementation is ready.

    if nargin==2
        while(1)
            [isDown,secs,code] = KbCheck(deviceNumber);
            if isDown
                if code(keyCode)
                    return;
                end
            end
            % Wait for 5 msecs to prevent system overload.
            WaitSecs('YieldSecs', 0.005);
        end
    elseif nargin==1
        while(1)
            [isDown,secs,code] = KbCheck();
            if isDown
                if code(keyCode)
                    return;
                end
            end
            % Wait for 5 msecs to prevent system overload.
            WaitSecs('YieldSecs', 0.005);
        end
    elseif nargin == 0
        error('Trigger key code must be specified in KbTriggerWait');
    elseif nargin > 2
        error('Too many arguments supplied to KbTriggerWait');
    end
end
