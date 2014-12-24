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
% architectures and specific versions of the X-Server,
% and thereby only for specific versions of specific
% Linux distributions. Installation of these drivers on
% a mismatched/unsuitable distribution will likely make
% your machines GUI unuseable by preventing successfull
% startup of the X-Server!
%
% For 64-Bit Ubuntu/KUbuntu/*buntu 14.04-LTS and compatible
% 64-Bit distributions with a XOrg 1.15 X-Server:
%
% FOR NVIDIA graphics cards:
%
% nouveau_drv.so_64BitForXOrg1-15-eg-Ubuntu14.04
%
% This is version 1.0.11 of the open source nouveau graphics
% driver for NVidia graphics cards. It contains many
% improvements over the v1.0.10 driver that is included in
% Ubuntu 14.04-LTS by default. This driver only works on
% 64-Bit distributions with a X-Server of the 1.15 series.
% It is only needed on Ubuntu 14.04-LTS. It won't work on
% earlier versions of Ubuntu and is not needed on later
% versions of Ubuntu, e.g., not needed on Ubuntu 14.10, as
% those versions already contain nouveau v1.0.11 or later.
%
% 1. Install it from a terminal window via:
%
% sudo cp nouveau_drv.so_64BitForXOrg1-15-eg-Ubuntu14.04 /usr/lib/x86_64-linux-gnu/xorg/extra-modules/nouveau_drv.so
%
% 2. Logout and login again, so the new driver gets loaded.
%
% The source code and license of nouveau-1.0.11 from which
% this binary driver was built can be found in the tar.gz
% file xf86-video-nouveau-1.0.11.tar.gz
%
%
% FOR INTEL graphics cards:
%
% intel_drv.so_64BitForXOrg1-15-eg-Ubuntu14.04
%
% This binary driver is built from intel-ddx driver version
% 2.99.917. It contains some useful bug fixes and multi-display
% improvements over the standard 2.99.910 driver that is included
% in Ubuntu 14.04-LTS by default. It also contains a modification
% to disable graphics triple-buffering by default, as this is
% hazardous to precise visual stimulus onset timing. This driver
% only works on 64-Bit distributions with a X-Server of the 1.15
% series. It is only needed on Ubuntu 14.04-LTS. It won't work on
% earlier or later versions of Ubuntu!
%
% 1. Install it from a terminal window via:
%
% sudo cp intel_drv.so_64BitForXOrg1-15-eg-Ubuntu14.04 /usr/lib/x86_64-linux-gnu/xorg/extra-modules/intel_drv.so
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
