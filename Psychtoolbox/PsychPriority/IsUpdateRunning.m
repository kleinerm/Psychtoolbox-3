function isRunning=IsUpdateRunning

% isRunning=IsUpdateRunning
%
% OS X: ___________________________________________________________________
%
% Report the number of processes named "update" which are running.  Only
% one update process at most should run but IsUpdateRunning will report
% higher values if indeed multiple update processes are running.
%
% Priority and Rush shut down the OS X update process when raising priority
% above 0. They use IsUpdateRunning to verify expectations about whether
% update is running prior to taking action.  
%
% OS 9: ___________________________________________________________________
%
% IsUpdateRunning does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% IsUpdateRunning does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: GetProcessList, GetRawProcesslist, Priority, Rush, finish.

% HISTORY
% 12/06/03  awi     wrote it.
% 7/15/03   awi     wrote it.

isRunning=length(GetProcessDescriptorFromCommandName('update'));
