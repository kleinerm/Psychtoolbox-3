% PsychtoolboxKernelDriver - A low level support driver for Apple OS-X
%
% The PsychtoolboxKernelDriver (PKD) is a MacOS-X kernel extension (a
% kext). It currently should work with AMD/ATI Radeon graphics cards of the
% X1000, HD2000, HD3000 and HD4000 series. It doesn't work with graphics chips from
% other manufacturers and probably doesn't work with older ATI chips. It
% won't work with the most recent HD5000 series chips and later.
%
% The driver needs to be manually loaded by a user with administrator
% privileges and provides a few special services to PTB-3, ie., PTB's
% functionality is extended/enhanced if it detects such a driver at startup
% time:
%
% * Beamposition queries (See help BeampositionQueries) allow for
% especially robust and accurate stimulus onset timestamping. They are
% supported by OS-X on all PowerPC systems and on Intel based systems with
% NVidia graphics cards, but not on Intel systems with ATI graphics. The
% driver restores this functionality on single display and dual display
% setups.
%
% * Dualhead display sync: The driver allows to synchronize the display
% refresh cycles of the two displays connected to a dualhead graphics card
% to allow for high quality tear-free binocular and stereo stimulation of
% subjects.
%
% * Use of 10 bit per color component framebuffers: The driver enables two
% extra bits of color output precision per color channel on your card,
% allowing for 1 billion shades of different colors instead of the 16.8
% million colors available without the driver.
%
% * More features to come in the future...
%
%
% The driver only works if a *single* graphics card (single-head or
% dual-head) is installed in your machine. It won't work reliably if more
% than one card is installed or more than 2 displays are driven by that one
% card.
%
% As this driver accesses the hardware at a very low level, bypassing the
% whole operating system, graphics subsystem and the drivers from ATI,
% there is some potential for things to go wrong. Although our testing so
% far didn't show any such problems, it may happen on your system. That is
% why this driver is an *experimental* feature and why you need to have
% administrator privileges to enable/load the driver.
%
% How to install (one time setup):
%
% Unzip the file Psychtoolbox/PsychAlpha/PsychtoolboxKernelDriver.zip, then
% copy the driver PsychtoolboxKernelDriver.kext into a system folder, e.g.,
% /System/Libary/Extensions/
% The location doesn't really matter, but that is the common place for such
% .kexts. You must do this from the terminal via:
%
% sudo cp -R PsychtoolboxKernelDriver.kext /System/Library/Extensions/
%
% assuming the unzipped driver is in your current working directory.
%
% How to enable (each time after restarting your system):
%
% In a terminal type:
% sudo kextload /System/Library/Extensions/PsychtoolboxKernelDriver.kext
%
% Both sudo commands will ask you for your password.
%
% USE THIS DRIVER AT YOUR OWN RISK - BUGS OR MALFUNCTIONS MAY CRASH YOUR
% MACHINE AND CAUSE DATA LOSS!
%
