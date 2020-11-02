% Psychtoolbox:PsychHardware:BrightSideDisplay
%
% Support files for interfacing Psychtoolbox-3 with the
% High Dynamic Range display device DR37-P or compatible
% devices from BrightSide Technologies.
%
% See http://www.brightsidetech.com
%
% NOTE: Deprecated and scheduled for removal. The company BrightSide
%       is dead since before the year 2010, and so are most likely
%       all DR37-P displays ever made. Therefore the code here is
%       almost certainly obsolete. Also, the driver mex file is no
%       longer bundled since many years, given it was only built
%       for 32-Bit Intel cpu's under WindowsXP for 32-Bit Matlab.
%
% The driver consists of a high-level M-File "BrightSideHDR.m"
% which implements the user callable PTB interface and a low-
% level Matlab MEX file "BrightSideCore.dll" that does the
% low level interfacing to BrightSides libraries.
%
% The runtime libraries and config files for your display
% need to be stored in the BSRuntimeLibs subfolder. They
% are not included in Psychtoolbox, so you need to manually
% copy them from your BrightSide installation CD to that folder.
%
% Have a look at SimpleHDRDemo.m to see how simple it is to use the
% display with PTB, thanks to the new PTB built-in imaging pipeline.
%
% Files:
%
% BrightSideBasicDemo   -- Basic demo to demonstrate use of the BrightSide display.
%
% BrightSideHDR.m       -- Psychtoolbox interface to the display.
% BrightSideCore.m      -- Technical documentation for BrightSideCore.mexw32.
% BrightSideCore.cpp    -- C++ source code for BrightSideCore.mexw32
%
% BSRuntimeLibs/        -- Subfolder for runtime libraries and config.
%                          Empty by default. Needs to be filled with files that
%                          are bundled with the software for your display device.
%
