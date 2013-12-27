% PsychVideoCapture -- Video capture support
%
% Psychtoolbox has built-in Screen subfunctions that allow you to control
% and use standard and professional video capture equipment to capture live
% video from a camera or other supported video source in real-time with low
% latency.
%
% This is useful for studies on (manipulated) visual feedback, e.g.,
% action-perception studies, as well as for future applications like vision
% based eye-trackers.
%
% The M-Files in this folder use the Screen - Low-level functions to
% provide more convenient high-level access to standard video capture
% tasks, e.g., setup of the camera or a video feedback loop with low and
% controlled latency.
%
% The functions are well tested and known to work perfectly on Linux.
% Support on MacOS-X and Windows is not always as feature rich, flexible
% and mature as on Linux. Especially, the OS-X and Windows versions are not
% as extensively tested. Some of the functions here are work in progress,
% useful, but not finished.
%
% Screen() supports two separate built-in videocapture engines for
% different purposes:
%
% PROFESSIONAL CLASS ENGINE FOR DEMANDING APPLICATIONS:
%
% For demanding pro-applications that require precise low-level control
% over your camera's features and capture parameters, high capture
% framerates, image resolutions, color depths and processing efficiency,
% low-latency, high timing precision and high capture timestamp precision,
% Screen implements a firewire videocapture engine which is based on the
% free-software and open-source cross-platform library "libdc1394". This
% engine supports professional IEEE-1394 machine vision cameras conforming
% to the IIDC 1.0 machine vision camera specification. These can be
% connected via IEEE-1394 firewire bus, or for some special examplars also
% via high-performance USB bus via "Firewire-over-USB" protocol. This
% high-perf capture engine is selected as default via the following Screen
% preference setting:
%
% Screen('Preference', 'DefaultVideocaptureEngine', 1);
%
% Alternatively you can select it on a case-by-case basis by passing the
% value 1 as optional 'engineID' parameter for the
% Screen('OpenVideocapture', ...); command.
%
% See <http://damien.douxchamps.net/ieee1394/libdc1394/> for information
% about the libdc1394 library, forums and links to the IIDC-Spec.
%
% A list of supported firewire pro-cameras can be found here:
% <http://damien.douxchamps.net/ieee1394/cameras/>
%
% The firewire engine is currently supported on Linux, where it was
% originally developed, extensively tested and used, and on MacOSX, where
% it received some light testing and use. Therefore, the Linux version is
% the most mature and well-tested one. It allows very convenient and
% fine-grained control over many aspects and settings of the cameras, it
% reliably can drive multiple cameras in parallel (tested with two cameras)
% and it has excellent timing, very low capture latency and highly accurate
% built-in timestamping code. The reported timestamps are accurate to a few
% dozen microseconds.
%
% You will need Linux kernel 2.4.21 or later or Linux 2.6.16 or later for
% best performance, but these are part of any recent distribution.
%
% Supported Cameras:
%
% All IIDC compliant cameras should work. For the Basler A602f greyscale
% high performance camera, the Basler A312fc, and the cheap and good
% Unibrain Fire-i camera, the PsychCamSettings - Tool provides especially
% convenient access to the camera settings.
%
% You can find additional setup instructions for the libdc1394 engine in
% 'help VideoCaptureDC1394'. Executing VideoCaptureDC1394 on OSX will
% install the runtime library for you.
%
%
% CONSUMER LEVEL ENGINE FOR LESS STRINGENT REQUIREMENTS:
%
% This engine is targeted at standard consumer class video capture and
% video digitizer equipment, e.g., built-in cameras of Laptop computers,
% USB or Firewire connected "webcams", DV camcorders, standard video
% converters and receivers etc. Quality of feature control, efficiency,
% attainable framerates and resolutions, precision of timing or capture
% timestamps and other properties vary on a model-by-model,
% vendor-by-vendor, operating-system-by-operating-system basis. While some
% relatively cheap cameras work very well on some operating systems, e.g.,
% the "Sony PlayStation Eye" USB web-camera, no guarantess can be made
% about the performance or quality of any specific camera.
%
% Screen supports a GStreamer based engine on all operating systems.
%
% We use GStreamer's built-in videocapture functionality by default (see
% "help GStreamer"). This is highly reliable and feature rich on Linux,
% somewhat less feature rich and reliable but still decent on Windows and
% OSX -- highly dependent on the version of Windows you are using. The
% engine should support most commercially available consumer level cameras
% for firewire, USB, PCI and other busses, basically any camera for which
% the operating system and GStreamer provide a device driver.
%
% This engine has id 3 and is selected by default on all operating systems.
% You can find additional setup instructions for GStreamer in 'help
% GStreamer'.
%
%
% Contents of this folder:
%
% libdc1394.22.dylib - The runtime library for the libdc1394 firewire video
%                      capture engine for 64-Bit MacOS/X. For installation
%                      into the /usr/local/lib/ system folder of your
%                      machine. Source code for this LGPL-v2+ licensed
%                      library can be found in the PTB source distribution
%                      ("help UseTheSource") in the following subfolder:
%                      PsychSourceGL/Cohorts/libDC1394
%
% PsychCamSettings   - Function for programmatically querying and setting
%                      camera parameters like exposure time, gain, brightness
%                      color saturation and such. Can also estimate the
%                      internal latency of the camera for known models,
%                      currently Basler A602f, A312fc and Unibrain Fire.i
%
% PsychGetCamIdForSpec - Return deviceIndex of a specified camera.
%
% PsychOpenEyes        - INCOMPLETE and therefore DYSFUNCTIONAL! Control
%                        interface for PTB's integrated vision based
%                        eyetracker, based on the OpenEyes toolkit.
%
% PsychSetupCamera     - Interactive tool for setting up a camera and writing
%                        the final settings into a .mat file for later use
%                        by experiment scripts.
%
% PsychVideoDelayLoop  - Full, feature rich implementation of a live
%                        video feedback loop with controllable latency.
%                        See its help for a list of features. See
%                        VideoDelayLoopMiniDemo for a demo of it.
%
