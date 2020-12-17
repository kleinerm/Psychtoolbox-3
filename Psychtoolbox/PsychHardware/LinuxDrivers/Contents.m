% Psychtoolbox/PsychHardware/LinuxDrivers
%
% Customized device drivers for Linux.
%
% The subfolder NVidiaOptimus/ contains a customized
% display modesetting driver and configuration files
% for 64-Bit X-Server 1.19, to use NVidia Optimus
% Laptops with the proprietary NVidia graphics driver
% instead of the open-source nouveau driver.
%
% CAUTION: These drivers are only for specific processor
% architectures and versions of the X-Server, and thereby
% only for specific versions of specific Linux distributions.
%
% Installation of these drivers on a mismatched/unsuitable
% distribution will likely make your machines GUI unuseable
% by preventing successfull startup of the X-Server!
%
% You can find out which version of X-Server is used on your
% system by typing...
%
% xdpyinfo | grep 'X.Org version'
%
% ... into a terminal window. It should report version 1.19 for a
% XOrg 1.19 server.
%
% Currently the following drivers are provided:
%

