function [flavorNameString, priorityStruct] = MachGetPriorityFlavor

% OS X: ___________________________________________________________________
%
% MachGetPriorityFlavor returns the priority flavor mode of the main 
% MATLAB thread, either 'THREAD_TIME_CONSTRAINT_POLICY' or 
% 'THREAD_STANDARD_POLICY'.  
% 
% The second return argument, priorityStruct, is the  priority struct for
% the active priority flavor mode.
% 
% The priority flavor 'THREAD_PRECEDENCE_POLICY' is never returned
% because its status is implied  by either 'THREAD_TIME_CONSTRAINT_POLICY'
% or  'THREAD_STANDARD_POLICY':
% 
%   -If flavorNameString is 'THREAD_TIME_CONSTRAINT_POLICY', then then
%   'THREAD_PRECEDENCE_POLICY'and its 'importance' parameter  are ignored
%   by the Mach task scheduler.
% 
%   -If flavorNameString is 'THREAD_STANDARD_POLICY' then the 'importance'
%   parameter associated with 'THREAD_PRECEDENCE_POLICY' governs the 
%   relative allocation of CPU time between the main MATLAB thread and all
%   other threads of the MATLAB process.  
% 
% MachGetPriorityFlavor probes priority using MachGetPriorityMex.
%
% OS 9: ___________________________________________________________________
%
% MachGetPriorityFlavor does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% MachGetPriorityFlavor does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: Priority, Rush, MachGetPriorityMex, MachSetPriorityMex, 


% AUTHORS:
% Allen Ingling     awi     Allen.Ingling@nyu.edu
%
% HISTORY: 
% 8/13/03   awi     Wrote it.
% 7/16/03   awi     Improved documentation.  Added AssertOSX.


%Give a sensible error message and exit if this is not OSX
AssertOSX;

% Find the mode by requesting the current priority parameters of a
% particular policy flavor.  If MachGetPriorityMex returns not 
% the current priority paramters as requested but instead the default priority parameters 
% (as indicated by the isDefault flag in the returned struct) then 
% the thread is not in the specified policy flavor mode.
%

psTCP = MachGetPriorityMex('THREAD_TIME_CONSTRAINT_POLICY', 0);  % 0 flag means get the current mode, 1 flag means get the default mode.
psS = MachGetPriorityMex('THREAD_STANDARD_POLICY', 0);             

if ~psTCP.isDefault && psS.isDefault
    flavorNameString='THREAD_TIME_CONSTRAINT_POLICY';
    priorityStruct=psTCP;
    return
elseif psTCP.isDefault && ~psS.isDefault
    flavorNameString='THREAD_STANDARD_POLICY';
    priorityStruct=psS;
    return
elseif psTCP.isDefault && psS.isDefault
    error('MachGetPriorityMex reports both THREAD_STANDARD_POLICY and THREAD_TIME_CONSTRAINT_POLICY modes. Internal error ?');
elseif ~psTCP.isDefault && ~psS.isDefault
    error('MachGetPriorityMex reports neither THREAD_STANDARD_POLICY nor THREAD_TIME_CONSTRAINT_POLICY mode. Internal error ?');
end
