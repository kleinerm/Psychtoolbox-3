% PsychtoolboxKernelDriver - A low level support driver for Apple OS-X
%
% The PsychtoolboxKernelDriver (PKD) is a MacOS-X kernel extension (a
% kext). It currently should fully work with AMD/ATI Radeon graphics cards
% of the X1000, and HD2000 to HD8000 series and later. On NVidia and Intel
% cards, only beamposition queries for high precision timestamping are
% supported.
%
% The driver needs to be manually installed by a user with administrator
% privileges and provides a few special services to PTB-3, ie., PTB's
% functionality is extended/enhanced if it detects such a driver at startup
% time:
%
% * Beamposition queries (See help BeampositionQueries) allow for
% especially robust and accurate stimulus onset timestamping. They are
% no longer supported by OS-X 10.9 and later and were not supported at all
% on Intel based Macintosh computers with AMD/ATI or Intel graphics.
% The driver restores this functionality for NVidia and AMD/ATI gpu's.
% Support for Intel gpu's is disabled by default, as it can lead to system
% crash due to some unfixable incompatibility of Intel graphics hardware
% which is completely out of our control.
%
% * Multihead display sync: The driver allows to synchronize the display
% refresh cycles of the displays connected to a multihead graphics card
% to allow for high quality tear-free binocular and stereo stimulation of
% subjects. This is only supported on AMD/ATI graphics cards.
%
% * Use of 10 bit per color component framebuffers: The driver enables two
% extra bits of color output precision per color channel on your card,
% allowing for 1 billion shades of different colors instead of the 16.8
% million colors available without the driver. Only supported on AMD/ATI
% graphics cards of X1000 series and later.
%
% * The driver implements workarounds to fix some problems caused by
% graphics driver and operating system bugs when the graphics card is used
% with high color/luminance precision display devices like the CRS Bits+ or
% Bits# boxes, or the VPixx Inc. DataPixx and ViewPixx devices, and similar
% equipment. This is only supported on AMD/ATI graphics hardware.
%
% The driver only works with one single graphics card at a time. On a
% single-gpu system it will just work. On a MacBookPro hybrid-graphics
% system with an integrated intel gpu and a discrete NVidia or AMD gpu, it
% will automatically switch to use the proper gpu. On a multi-gpu system
% with multiple discrete gpu's, e.g., MacPro with multiple graphics cards
% installed, it will use the default gpu zero by default. You can ask it use a
% different gpu by calling the command PsychTweak('UseGPUIndex', gpuidx);
% to select gpu 'gpuidx' - numbering starts at zero. Then call clear
% Screen, so Screen() actually picks up the new setting. Simultaneous use
% of multiple gpu's is not supported at this time.
%
% As this driver accesses the hardware at a very low level, bypassing the
% whole operating system, its graphics subsystem and the drivers from AMD,
% there is some potential for things to go wrong. Although our testing so
% far didn't show any such problems, it may happen on your system. That is
% why this driver is an *experimental* feature and why you need to have
% administrator privileges to install and load the driver the first time.
%
% How to install (one time setup):
% --------------------------------
%
% There are two versions of the driver, one for 32-Bit OSX kernels, one for
% 64-Bit kernels. You can find out which one you need by typing:
% !uname -m
%
% If the output says "x86_64" you'll need the 64-Bit driver, otherwise the
% 32-Bit driver.
%
% For 32-Bit OSX kernel:
% ----------------------
%
% This driver can no longer be maintained or improved, but should work on old versions
% of OSX which used to use 32-Bit kernels, and on their supported hardware. It has
% known limitations on AMD Radeon HD-4000 graphics cards and earlier cards wrt.
% control of digital display output dithering. Do not use it on such graphics cards
% for tasks which potentially need dithering to be disabled.
%
% You must type this into the terminal:
%
% cd /System/Library/Extensions/
% sudo unzip /PathToPsychtoolbox/Psychtoolbox/PsychHardware/PsychtoolboxKernelDriver32BitLegacy.kext.zip
%
% "PathToPsychtoolbox" must be replaced with the path to the Psychtoolbox folder, e.g., if your
% Psychtoolbox is installed under /Users/kleinerm/Psychtoolbox, then the above command would
% look like this:
%
% sudo unzip /Users/kleinerm/Psychtoolbox/PsychHardware/PsychtoolboxKernelDriver32BitLegacy.kext.zip
%
% For 64-Bit OSX kernel:
% ----------------------
%
% For OSX versions older than 10.8 Mountain Lion, use the PsychtoolboxKernelDriver64BitLegacy.kext.zip,
% which can no longer be maintained or improved but should work for OSX 10.6 - 10.7 and their supported
% hardware. It has known limitations on AMD Radeon HD-4000 graphics cards and earlier cards wrt.
% control of digital display output dithering. Do not use it on such graphics cards
% for tasks which potentially need dithering to be disabled. Upgrade to at least OSX version 10.8.
%
% For OSX version 10.8 and later, use the PsychtoolboxKernelDriver64Bit.kext.zip, as exemplified
% here.
%
% You must type this into the terminal:
%
% cd /System/Library/Extensions/
% sudo unzip /PathToPsychtoolbox/Psychtoolbox/PsychHardware/PsychtoolboxKernelDriver64Bit.kext.zip
%
% "PathToPsychtoolbox" must be replaced with the path to the Psychtoolbox folder, e.g., if your
% Psychtoolbox is installed under /Users/kleinerm/Psychtoolbox, then the above command would
% look like this:
%
% sudo unzip /Users/kleinerm/Psychtoolbox/PsychHardware/PsychtoolboxKernelDriver64Bit.kext.zip
%
%
% On modern OSX systems, this will automatically load the driver after a
% few seconds, and also after each system restart. On older systems you
% have to load the driver manually - on new systems you could do this for
% debugging purposes: How to enable (each time after restarting your system):
%
% In a terminal type:
% sudo kextload /System/Library/Extensions/PsychtoolboxKernelDriver.kext
%
% How to upgrade with a more recent version:
% ------------------------------------------
%
% You can unload and delete the driver before a driver upgrade via:
%
% sudo kextunload /System/Library/Extensions/PsychtoolboxKernelDriver.kext
% sudo rm -R /System/Library/Extensions/PsychtoolboxKernelDriver.kext
%
% Then you can follow the instructions for installation above.
%
% If everything went well, Psychtoolbox will report availability (and its
% use) of the driver the first time the Screen() mex file gets loaded and
% used. If it doesn't happen immediately, type "clear Screen" to force a
% reload of Screen.
%
