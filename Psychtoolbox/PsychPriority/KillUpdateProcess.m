function wasRunning=KillUpdateProcess
% wasRunning=KillUpdateProcess
% 
% OS X: ___________________________________________________________________
%
% Kill the OS X update daemon.  Killing update prevents  interruption of
% MATLAB at 30-second intervals, enabling  the Psychtoolbox to run smooth,
% uninterrupted animations.
%
% Use Priority or Rush instead of KillUpdateProcess; You need not invoke
% KillUpdateProcess directly from your scripts because Priority and Rush do
% that for you.  
% 
%
%                          * * * * * * * * * 
% IMPORTANT: When first installing the Psychtoolbox on your computer, run
% the included simplepsychtoolboxsetup.sh script from an OS X terminal
% window.  By modifying system file "/etc/sudoers"
% simplepsychtoolboxsetup.sh gives MATLAB permission to shutdown the OS X
% update process. KillUpdateProcess will detect if MATLAB does not have
% permission to kill the update process and prompt you to run
% simplepsychtoolboxsetup.sh. 
%                          * * * * * * * * * 
%
% KillUpdateProcess first checks to see if an update process is running. If
% no update process is running, then KillUpdateProcess returns value 0 to
% indicate that no update process was running.  If an update process is
% running, then KillUpdateProcess kills the update process, reads the jobs
% list to verify that update was indeed killed, and returns value 1 to
% indicate that update was running.  If KillUpdateProcess can not kill
% update because simplepsychtoolboxsetup.sh has not been run, then it
% prompts within the MATLAB command window  to run
% simplepsychtoolboxsetup.sh, opens a terminal window, and issues
% instructions.    
%
% To insure that MATLAB does not exit without restarting the update
% process,  The Psychtoolbox script finish.m, which MATLAB automatically
% calls when exiting MATLAB, invokes StartUpdateProcess. KillUpdateProcess
% will fail to kill update if it does not detect a finish.m file on the
% MATLAB path which contains a call to StartUpdateProcess. 
% 
% KillUpdateProcess will fail to kill update under any of these conditions:
%   - There is no finish.m file on the MATLAB path 
%   - There is more than one finish.m file on the MATLAB path  
%   - There is a single finish.m file on the MATLAB path but it does not
%     contain a  StartUpdateProcess call.
%   - The system "/etc/sudoers" file does not give MATLAB permission to 
%     shutdown update.  If KillUpdateProcess detects that 
%     simplepsychtoolboxsetup.sh has not been run, then it reports this, prompts
%     to run simplepsychtoolboxsetup.sh, and returns the value Nan.   
% 
% The OS X update process is part of its BSD Unix foundation. For more
% information on update, see the Unix manual page for update. (Enter "man
% update" in the OS X Terminal window.)
% 
% OS 9: ___________________________________________________________________
%
% KillUpdateProcess does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% KillUpdateProcess does not exist in Windows.
% 
% _________________________________________________________________________
%
% See also: StartUpdateProcess, KillUpdateProcess, IsUpdateRunning

% HISTORY
% 1/8/04    awi     Wrote it.
% 1/16/04   awi     Improved comments.  Added AssertOSX.  
%                   Consider changing this to AssertUnix. 
% 1/16/04   awi     Replaced kill line.
% 8/21/05   awi     Return Nan in case of no permission to shutdown update.  
% 8/22/05   awi     Prepended explanation to notice that "Priority" evoked it.
% 8/24/05   awi     Deleted from help text paragraph stating that KillUpdateProcess
%                   requests su password. Mario's script which modifies the
%                   sudoers file obviates that.
%                   Explained simplepsychtoolboxsetup.sh in help.
%                   Cosmetic
% 10/10/05 awi		Merged with dgp version: changed "strcmp" to "streq"

% TO DO: 
% A better solution to the problem of insurring that the update process is
% eventually restored would be to provide a Cocoa application, launched by
% KillUpdateDameon, which checked for update and nagged if it went
% undetected for a time. If MATLAB were found to be not running it would
% make sure that update was running.  The same application could handle
% starting and stopping update.   Even if the app did not
% handle starting and stopping, this scheme could be made foolproof (for
% reasonable levels of foolishness) if the app would not exit unless update
% was running and matlab would not kill update unless the app were running.
% That way alwyas either update is running or the update restoration app is
% running. This update gurantor app could also have time limit after which
% it will restart update even if MATLAB were running.  Perhaps it could
% decide this on the basis of MATLAB idle time?  
% 

AssertOSX;

% check that the shell script to give everyone permission to shut down and
% restart the update process has been run.  It leaves its mark by backing
% up the /etc/sudoers file into  /etc/sudoers_pre_psychtoolbox.  This is
% not a perfect test, because someone could modify /etc/sudoers without
% deleting the /etc/sudoers_pre_psychtoolbox file. However, we can not
% examine the contents of sudoers without root.
if exist('/etc/sudoers_pre_psychtoolbox')~=2
    Screen('CloseAll');
    fprintf(' This message has been triggered by an attempt to raise priority of the MATLAB \n');
    fprintf(' process above level 0.  If you did not call "Priority" or "Rush" directly from the\n');
    fprintf(' MATLAB command line, then it may have been called within another script or function\n');
    fprintf(' which you did call\n');
    fprintf('\n');

	fprintf(' In order to use raise priority on OS X you first need to run the script\n');
	fprintf(' simplepsychtoolboxsetup.sh from the OS X Terminal command line. In OS X,\n');
	fprintf(' Priority and Rush commands require shutting down the system "update" process\n');
	fprintf(' for as long as priority remains above 0. Shutting down update ordinarily\n');
	fprintf(' requires entering an administrator password.  However, the Psychtoolbox\n');
	fprintf(' includes a shell script which will reconfigure your computer to allow MATLAB\n');
	fprintf(' to shutdown update without having to enter a password. Examining your /etc\n');
	fprintf(' directory, the absence of file "sudoers_pre_psychtoolbox " seems to indicate that\n');
	fprintf(' you have not run the simplepsychtoolboxsetup.sh script since installing\n');
	fprintf(' the Psychtoolbox.\n\n');
	fprintf(' Should I open a terminal window and give instructions ');
    yn=input(' [Y/N]? ','s');
    keepAsking=1;
    while(keepAsking)
        goAhead=streq(upper(yn), 'Y') || streq(upper(yn), 'YES');
        exitOut=streq(upper(yn), 'N') || streq(upper(yn), 'NO');
        keepAsking= ~(goAhead || exitOut);
    end %while 
    if exitOut
        wasRunning=nan;
        return;
    end
    unix('open -a Terminal');  
    fprintf('\n\n\nInstructions for calling the simplepsychtoolboxsetup.sh script within the\n');
    fprintf('terminal window:\n\n');
    fprintf('1. Find the full path to the simplepsychtoolboxsetup.sh script on your\n');
    fprintf('   hard drive. For example, if you keep your Psychtoolbox folder inside\n');
    fprintf('   your applications folder, then the path would be:\n');
    fprintf('      /Applications/Psychtoolbox/PsychContributed\n\n');
    fprintf('2. Use the unix cd command to move to the PsychContributed directory. For example:\n');
    fprintf('      cd /Applications/Psychtoolbox/PsychContributed\n\n');
    fprintf('3. Invoke the simplepsychtoolboxsetup.sh script using Sudo.  You will then be\n');
    fprintf('   prompted to enter an administrator password.  For example:\n');
    fprintf('      sudo ./simplepsychtoolboxsetup.sh\n\n\n');
    fprintf(' If you successfully run the simplepsychtoolboxsetup.sh script after installing\n');
    fprintf(' the Psychtoolbox, then you should thereafter be able to raise priority\n');
    wasRunning=nan;
    return;
end
% check that the command to restart update is implanted in the finish file
if ~IsStartUpdateImplantedInFinish
    error('Killing the update process first requires that StartUpdateProcess be called from the finish.m file.  See "help KillUpdateProcess".');
end
p=GetProcessDescriptorFromCommandName('update');
if isempty(p)
    wasRunning=0;
    return;
elseif length(p) > 1
    error('Multiple processes with command name "update" detected.  This is unexpected.');
else  %there is only one update process
%	unix([ 'sudo kill ' int2str(p.pid) ]);
    unix('sudo /usr/bin/killall update');
	pause(1);   %allow OS X lots of time to shut down update before checking. 
	p=GetProcessDescriptorFromCommandName('update');
	if ~isempty(p)
        %allow OS X lots more time to shut down update before checking. 
        pause(5);   
        p=GetProcessDescriptorFromCommandName('update');
	end
	if ~isempty(p)
        error('Failed to kill the update process');
	end
	wasRunning=1;
end %else




