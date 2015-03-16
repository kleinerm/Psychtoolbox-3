% Psychtoolbox/PsychHardware/LinuxDrivers
%
% Customized device drivers for Linux.
%
% This folder currently contains customized X11 graphics
% drivers for Linux which provide enhanced functionality
% or bug fixes not yet available in standard Linux
% distributions but useful for better visual stimulus
% presentation.
%
% CAUTION: These drivers are only for specific processor
% architectures and versions of the X-Server, and thereby
% only for specific versions of specific Linux distributions.
% Installation of these drivers on a mismatched/unsuitable
% distribution will likely make your machines GUI unuseable
% by preventing successfull startup of the X-Server!
%
% You can find out which version of X-Server is used on your
% system by typing...
%
% xdpyinfo | grep 'X.Org version'
%
% ... into a terminal window. It should report version 1.16 for a
% XOrg 1.16 server.
%
% Drivers for 64-Bit Ubuntu/KUbuntu/*buntu versions 14.04.2 LTS
% and 14.10 as well as other compatible 64-Bit distributions with a
% XOrg 1.16 X-Server:
%
% FOR INTEL graphics cards install intel_drv.so_64BitForXOrg1-16
%
% This binary driver is built from intel-ddx driver version 2.99.917.
% It contains some useful bug fixes and multi-display improvements
% over the standard 2.99.914 driver that is included in Ubuntu
% 14.04.2-LTS and 14.10 by default. It also contains a modification to
% disable graphics triple-buffering by default, as this is hazardous to
% precise visual stimulus onset timing.
%
% 0. If the file isn't available in your LinuxDrivers directory, you can simply
% download it from here:
%
% https://github.com/Psychtoolbox-3/Psychtoolbox-3/blob/master/Psychtoolbox/PsychHardware/LinuxDrivers/intel_drv.so_64BitForXOrg1-16?raw=true
%
% 1. Install it from a terminal window via:
%
% sudo cp intel_drv.so_64BitForXOrg1-16 /usr/lib/x86_64-linux-gnu/xorg/extra-modules/intel_drv.so
%
% 2. Logout and login again, so the new driver gets loaded.
%
% The source code (and license) from which this binary driver
% was built can be found in the tar.gz file ...
% xf86-video-intel-2-99-917+MKTripleBufferOff.tar.gz
% The patch which was applied to the original 2.99.917 source to
% disable triple buffering by default can be found in
% 0001-Disable-triple-buffering-by-default.patch
%
