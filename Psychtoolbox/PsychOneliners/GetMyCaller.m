function caller=GetMyCaller
% caller=GetMyCaller
%
% OS X and OS 9: ___________________________________________________________________
%
% GetMyCaller returns the name of its caller by using MATLAB's dbstack
% function.  If invoked from the command line, then GetMyCaller returns
% 'base'.  
%
% WINDOWS: ________________________________________________________________
% 
% GetMyCaller does not yet exist in Windows.
% 
% _________________________________________________________________________
%
% see also: dbstack

% 2004    awi Wrote it.
% 1/29/05 dgp Cosmetic.
% 10/24/05 awi Cosmetic.

[callStack, stackIndex]=dbstack;
callDepth=length(callStack);
if callDepth==1
    caller='base';
else
    fullCaller=callStack(2).name;
    [pathstr,caller,extension,version]=fileparts(fullCaller);
end

