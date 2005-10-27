function wasRunning = StartUpdateProcess

% wasRunning = StartUpdateProcess
%
% OS X: ___________________________________________________________________
%
% If the OS X update process is running then just return 1.  Otherwise
% start the update process and return 0.   
%
% Because the update process runs as root, StartUpdateProcess requires a
% password be entered.
%
% Note that StartUpdateProcess first checks to be sure that the update
% process is running before launching one, so there is no danger of it
% starting an additional update process if one is already running
%
% OS 9: ___________________________________________________________________
%
% StartUpdateProcess does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% StartUpdateProcess does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: IsUpdateRunning, Rush, Priority

% HISTORY
% 1/8/04    awi     Wrote it.
% 7/15/04   awi     Improved documentation.     


%first check to see if the update process is running
wasRunning=IsUpdateRunning;
if wasRunning>1
    % This should never happen.
    warning('More than one update process is running.  There should be only one.');
    return;
elseif wasRunning==1
    return;
else % it was not running
%     fprintf('The OS X system update process has been halted to prevent interruption of MATLAB during "Rush" and "Priority" commands.\n');
%     fprintf('If the update process is not running then the Psychtoolbox automatically restarts the update process when MATLAB\n');
%     fprintf('exits.  Starting the MATLAB process requires the system administrator password.  To restart update process, enter\n');
%     fprintf('the system administrator password at the prompt.  You can also start the update process by entering at the\n');
%     fprintf('Terminal shell prompt:\n');
%     fprintf('\tsudo update\n');
%     fprintf('From the shell you can check to see if the update process is running by issueing the command:\n');
%     fprintf('\tps -awx | grep update\n');
%     fprintf('\n');
    unix('sudo /usr/sbin/update');
end

    
