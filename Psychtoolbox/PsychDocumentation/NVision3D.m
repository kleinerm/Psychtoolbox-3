% NVision3D - Support for NVidia NVision stereo goggles.
%
% Psychtoolbox supports use of USB driven stereo shutter glasses from
% NVidia's NVision stereo kit. The support for USB is currently limited to
% 64-Bit Linux systems, ie., 64-Bit Octave and 64-Bit Matlab on Linux.
%
% If you want to drive such shutter glasses via a suitable NVidia Quadro
% series GPU or AMD Fire series GPU that has a 3-Pin-Mini-DIN stereo output
% connector then you can simply connect the stereo emitter to that
% connector, setup your graphics card for it, and select stereomode 1 in
% Psychtoolbox.
%
% If you want to drive such shutter glasses via any graphics card from any
% vendor via the USB connector, then you will have to select stereomode 11
% in your scripts and perform the following setup steps beforehand. This
% functionality is currently limited to 64-Bit Linux only, and highly
% experimental in nature, without any guarantees that this will work
% reliably - or at all - for you.
%
% Use of USB driven stereo goggles is enabled by use of the LGPL licensed
% open-source library nvstusb from ...
% <http://sourceforge.net/projects/libnvstusb> ... thank you!
% A copy of the libraries source code can be found in the
% PsychSourceGL/Cohorts/libnvstusb-code-32/ subfolder or the original code
% in the libnvstusb-code-32.zip file in that folder. See "help
% UseTheSource" on how to get the PsychSourceGL folder with Psychtoolbox
% source code.
%
%
% Setup steps:
%
% 1. Run PsychLinuxConfiguration if you installed PTB from your
%    Linux distributions package manager, or from NeuroDebian. If you
%    installed via DownloadPsychtoolbox or UpdatePsychtoolbox,
%    PsychLinuxConfiguration will have been executed for you already. Make
%    sure to answer with yes when the function asks you for approval to
%    update the udev rules file.
%
% 2. Reboot your machine.
%
% 3. Connect the stereo emitter to a suitable USB port.
%
% 4. Get the nvstusb.fw file from somewhere. You can find it either
%    on the Internet via a Google search, or you can use the
%    nvstusb-extractfw tool in the Psychtoolbox/PsychHardware folder from a
%    terminal window. It needs to be pointed to a file of the installed
%    NVidia stereo driver for Microsoft Windows, which obviously requires
%    you to have a working MS-Windows installation with a NVidia card.
%    Alternatively follow the instructions about generating nvstusb.fw from
%    this website: <http://nvstusb.gnu-log.net/>
%
% 5. Copy the nvstusb.fw file into your home directory, e.g., if your
%    user name is mathilde: /home/mathilde/nvstusb.fw or even better into
%    the Psychtoolbox configuration directory, e.g.,
%    /home/mathilde/.Psychtoolbox/nvstusb.fw
%
% 6. You should be set. Run, e.g., StereoDemo(11) or ImagingStereoDemo(11).
%
