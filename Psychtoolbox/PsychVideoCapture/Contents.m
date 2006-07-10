% PsychVideoCapture -- Video capture support
%
% Psychtoolbox has built-in Screen-Subfunctions that allow you
% to control and use standard video capture equipment to capture
% live video from a camera or other supported video source in
% real-time with low latency.
%
% This is useful for studies on (manipulated) visual feedback,
% e.g., action-perception studies, as well as for future
% applications like vision based eye-trackers.
%
% The M-Files in this folder use the Screen - Low-level functions
% to provide more convenient high-level access to standard video
% capture tasks, e.g., setup of the camera or a video feedback
% loop with low and controlled latency.
%
% THIS IS EXPERIMENTAL CODE FOR NOW! It is well tested and known
% to work perfectly on Linux, but support on MacOS-X and Windows
% is not yet as feature rich, flexible and mature as on Linux.
% Especially, the OS-X and Windows versions are not as extensively
% tested. Some of the functions here are work in progress, useful,
% but not finished.
%
% OS-X & Windows: We use the Sequence-Grabber API of Apples Quicktime
% for video capture on OS-X and Windows. On Windows, you'll need
% to install a Quicktime compatible video digitizer component, e.g.,
% WinVDIG, which can be freely downloaded from http://www.vdig.com/WinVDIG/
%
% On MacOS-X 10.4, video digitizers are already installed.
%
% Windows and MacOS-X support USB-Webcams, Firewire digital cameras
% conforming to the IIDC 1.0 machine vision camera specification,
% standard framegrabber cards and standard digital video cameras.
% -> Everything that is supported by the operating system.
%
% On Linux, currently only Firewire digital cameras are supported that
% conform to the IIDC 1.0 machine vision camera specification.
% Internally we use the excellent and free LibDC1394 library. See
% http://damien.douxchamps.net/ieee1394/libdc1394/index.php
% for information, forums and links to the IIDC-Spec.
%
% A list of supported cameras can be found here:
% http://damien.douxchamps.net/ieee1394/cameras/index.php
%
% The Linux version is the most mature and well-tested one. It allows
% very convenient and fine-grained control over many aspects and
% settings of the cameras, it reliably can drive multiple cameras
% in parallel (tested with two cameras) and it has excellent timing,
% very low capture latency and highly accurate built-in timestamping
% code. The reported timestamps are accurate to a few dozen microseconds.
%
% You will need Linux kernel 2.4.21 or later or Linux 2.6.16
% or later for best performance, but these are part of any recent
% distribution.
%
% Supported Cameras on Linux:
%
% All IIDC compliant cameras should work. For the Basler A602f greyscale
% high performance camera and the cheap and good Unibrain Fire-i camera,
% the PsychCamSettings - Tool provides especially convenient access to
% the camera settings.
%
% Supported cameras on OS-X/Windows:
%
% All cameras supported by the operating system are supported, but there
% is only very limited support for changing camera settings and accessing
% special features. Capture works reliably, but the capture latency is
% pretty much unknown and the timing mechanisms for retrieving capture
% timestamps are hidden in Apples Quicktime implementation and therefore
% their reliability and internal working is unknown.
%
% Contents:
%
% PsychCamSettings - Function for programmatically querying and setting
%                    camera parameters like exposure time, gain, brightness
%                    color saturation and such. Can also estimate the
%                    internal latency of the camera for known models,
%                    currently Basler A602f and Unibrain Fire.i
%
% PsychSetupCamera - Interactive tool for setting up a camera and writing
%                    the final settings into a .mat file for later use
%                    by experiment scripts.
%
% PsychVideoDelayLoop - Full, feature rich implementation of a live
%                       video feedback loop with controllable latency.
%                       See its help for a list of features.
%
