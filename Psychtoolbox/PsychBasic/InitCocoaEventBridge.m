function InitCocoaEventBridge

% OS X: ___________________________________________________________________
%
% Check to see if the Psychtoolbox mex function "CocoaEventBridge" knows the
% path to the "GetCharWindow.bundle" bundle which it loads.  If it does not, then tell
% CocoaEventBridge the path: The GetCharWindow bundle is in the same
% directory as CocoaEventBridge.  
%
% InitCocoaEventBridge may be called generously, becasue it first checks
% CocoaEventBridge to see if it knows the path before setting the path.
%
% All Psychtoolbox functions which use CocoaEventBridge call
% InitCocoaEventBridge as needed; There should be no need to call it in
% your functions and scripts. 

% 7/19/05 awi   Wrote it.  


% NOTES:
%
%  Possible variant implementations of this sort of initializing, with some
%  comments on their relative merits:
%   
%  - The CocoaEventBridge mex file could make callbacks to the MATLAB
%  environment to get the path.  Not absolutly certain, but I think there is some
%  conflicet between callbacks and that compiler which builds standalone
%  apps from MATLAB scripts.  Supporting that compiler is a minor priority, but 
%  we do get requests for that capability, so why not help those people out
%  here at no cost and avoid the callback ?  
%
%  - The check to see if InitCocoaEventBridge has set the path could  
%  query  a global variable,  not query InitCocoaEventBridge itself.
%  But if we querie the mex file itself, there is no way to flush the state
%  of the path separately from the mex file.  
% 
%  --awi


if(IsOSX)
    oldPath=CocoaEventBridge('PathToBundle');
    if strcmp(oldPath, '') 
        cocoaBundleFileName= 'GetCharWindow.bundle';
        [fPathStr, fName, fExtension, fVersion]=  fileparts(which('CocoaEventBridge'));
        pathToGetCharWindowBundle= fullfile(fPathStr, cocoaBundleFileName);
        CocoaEventBridge('PathToBundle', pathToGetCharWindowBundle);
    end
end






