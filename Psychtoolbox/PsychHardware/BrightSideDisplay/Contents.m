% Psychtoolbox:PsychHardware:BrightSideDisplay
%
% EARLY PROTOTYPE - NOT READY FOR GENERAL USE!
%
% Support files for interfacing Psychtoolbox-3 with the
% High Dynamic Range display device DR37-P or compatible
% devices from BrightSide Technologies.
%
% See http://www.brightsidetech.com
%
% The driver consists of a high-level M-File "BrightSideHDR.m"
% which implements the user callable PTB interface and a low-
% level Matlab MEX file "BrightSideCore.dll" that does the
% low level interfacing to BrightSides libraries.
%
% The runtime libraries and config files for your display
% need to be stored in the BSRuntimeLibs subfolder. They
% are not included in Psychtoolbox, but you need to manually
% copy them from your BrightSide installation CD to that folder.
%
% Files:
%
% BrigthSideDemo.m      -- Simple demo to demonstrate use of these functions.
% BrightSideHDR.m       -- Psychtoolbox interface to the display.
% BrightSideCore.dll    -- Matlab MEX interface, used by BrightSideHDR.
% BrightSideCore.m      -- Technical documentation for BrightSideCore.dll.
% BrightSideCore.c      -- C++ source code for BrightSideCore.dll.
% BSRuntimeLibs/        -- Subfolder for runtime libraries and config.
%                          Empty by default.
%
