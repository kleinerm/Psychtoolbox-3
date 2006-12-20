% Psychtoolbox:PsychHardware:BrightSideDisplay
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
% are not included in Psychtoolbox, so you need to manually
% copy them from your BrightSide installation CD to that folder.
%
% Have a look at SimpleHDRDemo.m to see how simple it is to use the
% display with PTB, thanks to the new PTB built-in imaging pipeline.
%
% Files:
%
% SimpleHDRDemo.m       -- Simple demo to demonstrate use of BrightSide-HDR with
%                          Psychtoolbox.
%
% ShowHDRDemo.m         -- Less simple demo to demonstrate use of these functions.
%
% HDRViewer.m           -- An interactive viewer for HDR images, including
%                          zoom function.
%
% BrightSideHDR.m       -- Psychtoolbox interface to the display.
% BrightSideCore.dll    -- Matlab MEX interface, used by BrightSideHDR.
% BrightSideCore.m      -- Technical documentation for BrightSideCore.dll.
% BrightSideCore.cpp    -- C++ source code for BrightSideCore.dll.
%
% BSRuntimeLibs/        -- Subfolder for runtime libraries and config.
%                          Empty by default. Needs to be filled with files that
%                          are bundled with the software for your display device.
%
% HDRRead.m             -- Generic HDR image reader. Dispatches into helper
%                          routines for different file formats.
%
