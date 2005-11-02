function DownloadPsychtoolbox(flavor, targetdirectory)
% DownloadPsychtoolbox(branchid, targetdirectory)
%
% Download the Psychtoolbox for OS-X via the Subversion client from the
% Subversion master repository and create a working copy suitable for
% automatic software updates.
%
% This function will download a working copy of the Psychtoolbox for OS-X,
% either into the current working directory, or into 'targetdirectory' at
% your option.
%
% You can specify the flavor of Psychtoolbox release via the flavor -
% parameter:
%
% flavor == 'stable' - Download the most recent fully tested production
% release. This is the code that will constitute the next official PTB
% release - well tested by developers and by users in real world
% application.
%
% flavor == 'beta' - Download the most recent beta release. This is code
% that has been successfully tested by the developers and is ready for
% testing and feedback by end users. Beta-code that has received
% enough positive feedback from end users will be promoted to the 'stable'
% branch and finally into the next official release.
%
% flavor == 'Psychtoolbox-1.x.y' - Download the code which corresponds
% to official release Psychtoolbox-1.x.y . This is identical to the
% packages that can be downloaded from www.psychtoolbox.org.
%
% We recommend to either use the 'stable' flavor, or the 'beta' flavor if
% you like pre-release code with early access to bug fixes, enhancements
% and new features.
%
% How to use:
%
% 1. Download the latest MacOS-X Subversion client from:
% http://metissian.com/projects/macosx/subversion/
%
% 2. Install it on your machine by double-clicking the installer and
% following the instructions.
%
% 3. Change your Matlab working directory to the location where you want to
% install Psychtoolbox.
%
% 4. Run this file, either as DownloadPsychtoolbox('stable') or as
% DownloadPsychtoolbox('beta') at your choice.
%
% 5. After sucessfull installation, setup your Matlab path properly, so
% this working copy of Psychtoolbox gets used.
%
% 6. You are done for now. Enjoy!
%
% If you want to upgrade your copy of Psychtoolbox in the future to incorporate the
% latest bug fixes, enhancements and new features, please do the following:
%
% 1. Change your Matlab current directory to the Psychtoolbox - folder.
%
% 2. Call the script: 'UpdatePsychtoolboxFromSVN'.
%
% 3. Your working copy will get updated to the latest release.
%
% 4. Enjoy the new version.

% History:
%
% 11/2/05 mk  Created.

if nargin < 2
    targetdirectory = pwd;
end;

if nargin < 1
    flavor = 'stable';
end;

% Check if subversion client is properly installed:
if exist('/usr/local/bin/svn', 'file')~=2
    fprintf('Could not find the Subversion client "svn" in its\n');
    fprintf('expected location /usr/local/bin/svn ! Please download\n');
    fprintf('and install the most recent Subversion client from...\n\n');
    fprintf('http://metissian.com/projects/macosx/subversion/\n\n');
    fprintf('... and then retry. Ciao.\n');
    return;
end;

fprintf('Trying to download the latest working copy of Psychtoolbox-OSX.\n');
fprintf('Target folder for installation: %s\n', targetdirectory);
fprintf('Requested flavor is: %s\n', flavor);
fprintf('\n');

checkoutcommand = char([ '/usr/local/bin/svn checkout svn://svn.berlios.de/osxptb/' flavor '/Psychtoolbox/' ]);

fprintf('Will execute the following checkout command, please standby...\n');
fprintf('%s\n', checkoutcommand);

[status, result] = system(checkoutcommand);
if (status>0)
    fprintf('Operation failed for some reason. Sorry...\n');
end;

fprintf('Operation finished! Output of svn is as follows...\n');
fprintf('%s\n', result);

fprintf('Please adapt your Matlab path to include the folder\n');
fprintf('%s/Psychtoolbox\n', targetdirectory);
fprintf('and all its subfolders. Then you are ready to use the Psychtoolbox - Enjoy!\n');




