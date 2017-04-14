% PsychtoolboxKernelDriver - A low level support driver for Apple OS-X
%
% The PsychtoolboxKernelDriver (PKD) is a MacOS-X kernel extension (a
% kext). It currently should fully work with AMD/ATI Radeon graphics cards
% of the X1000 series, as well as the HD2000 series up to the most recent
% AMD graphics cards, iow. anything released since around the year 2006.
% On NVidia cards, only beamposition queries for high precision timestamping
% are supported. Intel graphics chips are not supported.
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
% crashes due to some unfixable incompatibility of Intel graphics hardware
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
% million colors available without the driver. This was only supported on
% AMD/ATI graphics cards and is no longer supported since Psychtoolbox 3.0.14,
% as Apple sabotaged our ability to provide this functionality on OSX 10.11
% and later.
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
% whole operating system, its graphics subsystem and drivers, there is
% some potential for things to go wrong. Although our testing over 10 years
% didn't show any such problems, it could happen on some future OSX systems.
% That is why this driver is considered a permanently experimental feature.
%
% How to install (one time setup):
% --------------------------------
%
% OSX versions older than 10.11 "El Capitan" are no longer supported.
%
% Install the PsychtoolboxKernelDriver64Bit.kext.zip, as exemplified here.
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
% This will automatically load the driver after a few seconds, and also after
% each system restart.
%
% If the driver fails to load, then please report it to the Psychtoolbox forum.
% If it fails to load due to invalid cryptographic signatures you can disable
% "System integrity protection" (SIP) as a temporary measure. The Internet will
% tell you how to do that. This would make the kernel driver load, but reduce
% resistance of your system against potential hacker/virus/trojan horses attacks
% and other security threats.
%
% How to upgrade with a more recent version, bundled with a new version of Psychtoolbox:
% --------------------------------------------------------------------------------------
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
