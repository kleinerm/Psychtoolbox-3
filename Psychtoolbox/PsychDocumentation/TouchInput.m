% TouchInput - Psychtoolbox support for touch input devices.
%
% Psychtoolbox currently best supports touch input devices like
% touchscreens and touchpads on Linux with the classic X11
% X-Display system, ie. when running under a classic X-Server.
%
% More basic/limited/inflexible support is also available for Microsoft
% Windows 10 and later.
%
% The way you use such devices to get touch input, is as follows:
%
% 1. Find the device(s) to use for touch input via GetTouchDeviceIndices().
%
% 2. Get basic information about a device of choice via GetTouchDeviceInfo().
%
% 3. Create (TouchQueueCreate()) a touch input queue for the selected device.
%
% 4. Use TouchQueueStart(), TouchQueueStop() and TouchEventFlush() to start,
%    and stop data collection, or to discard all currently pending touch input.
%
% 5. Use TouchEventAvail() to find the number of collected touch events, and
%    TouchEventGet() to retrieve events. These events provide all the basic
%    information of interest: The time when the event was received, the (x,y)
%    touch coordinates in different useful coordinate systems, if it was a
%    start of a new touch, the end of an old touch, or some state change on an
%    existing touch, e.g., position, pressure, shape etc. Additional device
%    and operating system specific info (non-portable!) can be retrieved via
%    the helper function GetTouchValuators().
%
% See the help for the functions mentioned above for more details. The demo
% MultiTouchDemo.m demonstrates most of this functionality.
%
% MultiTouchMinimalDemo.m shows a more basic demo of how to get the essentials.
%
% You can measure the latency and timing accuracy with KeyboardLatencyTest(),
% selecting a modality setting of 10 and using a soundcard + microphone.
% The latency of touch screens varies greatly across vendors and models.
% For reference, the ELAN touchscreen built into the flat-panel of a tested
% RazerBlade 2016 gaming laptop had a latency as low as 16 msecs for reporting
% touches, with a low standard deviation of 2 msecs, but the Synaptics TM2438-005
% touchpad in the same laptop had a latency of about 130 msecs [stddev ~4 msecs].
%
% A tested Apple Magic touchpad fared at about 50 msecs, an Apple MacBookPro
% mouse touchpad at about 25 msecs with over 4 msecs standard deviation.
%
% While touchscreens should just work as expected, touchpads are not designed
% for "touchscreen-like" touch input. If you want to get them to act as mini-
% touchscreens, you will have to disable use of the "synaptics" touchpad driver,
% e.g., by uninstalling the xf86-input-synaptics package, or by removing or
% editing or overriding the synaptics configuration file. This file can be
% found in the directory /usr/share/X11/xorg.conf.d/ on many systems. After
% modifying the system this way and logging out and logging in again, you
% should have a touchpad that no longer works for controlling a mouse pointer,
% but can work as a minimalistic touchscreen replacement. Alternatively read on
% for a way to override the default settings.
%
% On systems which use the xserver-xorg-input-libinput driver for touch input, the
% amount of information provided about each touch point is somewhat limited, at least
% as of the state of Ubuntu 17.10. If you want maximally detailed information about
% touch input, you should instead install the xserver-xorg-input-evdev driver and then
% override the system default input configuration to use the evdev input driver
% for touchscreens. You can do this easily by copying the file ...
%
% Psychtoolbox/PsychHardware/99-evdev-touchscreen.conf
%
% ... into the folder /etc/X11/xorg.conf.d/ and then logout and login again. This
% would activate evdev for maximum detail in touch input reporting, e.g., to provide
% info about the shape, size and orientation of a touch point, the type of touchpoint
% (finger, palm, some digitizer pen or tool), the distance of the tool from the touch
% surface, or the pressure exerted onto the touch surface by a tool or finger.
%
% This sample configuration file also contains a commented out section that allows
% to disable the synaptics driver for touchpads and instead also use evdev for them,
% to allow their (ab)use as mini touchscreens, while disabling their regular touchpad
% functionality. All you'd have to do is comment that section in again, logout and
% login once.
%