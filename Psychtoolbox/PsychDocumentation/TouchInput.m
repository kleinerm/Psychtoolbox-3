% TouchInput - Psychtoolbox support for touch input devices.
%
% Psychtoolbox currently supports touch input devices like
% touchscreens and touchpads on Linux with the classic X11
% X-Display system, ie. when running under a classic X-Server.
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
% MultiTouchDemo.m demonstrates most of this functionality. You can measure the
% latency and timing accuracy with KeyboardLatencyTest(), selecting a modality
% setting of 10 and using a soundcard + microphone.
%
% While touchscreens should just work as expected, touchpads are not designed
% for "touchscreen-like" touch input. If you want to get them to act as mini-
% touchscreens, you will have to disable use of the "synaptics" touchpad driver,
% e.g., by uninstalling the xf86-input-synaptics package, or by removing or
% editing or overriding the synaptics configuration file. This file can be
% found in the directory /usr/share/X11/xorg.conf.d/ on many systems. After
% modifying the system this way and logging out and logging in again, you
% should have a touchpad that no longer works for controlling a mouse pointer,
% but can work as a minimalistic touchscreen replacement.
%
