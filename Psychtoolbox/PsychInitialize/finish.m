% finish
% 
% From MATLAB documentation: 
%
%   When MATLAB quits, it runs a script called finish.m, if it exists and
%   is on the MATLAB search path or in the current directory. This is a
%   file that you create yourself in order to have MATLAB perform any final
%   tasks just prior to terminating.
%
% OS X: ___________________________________________________________________
%
%   Finish checks to see if the OS X update process is running and restarts
%   it if it is not.  Priority and Rush shut down the update process when
%   raising Priority above 0.
%
% OS 9: ___________________________________________________________________
%
%   The Psychtoolbox does not provide finish.m.
%
% WINDOWS:_________________________________________________________________
%
%   The Psychtoolbox does not provide finish.m.
%
% _________________________________________________________________________
%
%  see also: PsychPriority, PsychRush, IsUpdateRunning, IsStartUpdateImplantedInFinish 


if(IsOSX)
    StartUpdateProcess;
end
