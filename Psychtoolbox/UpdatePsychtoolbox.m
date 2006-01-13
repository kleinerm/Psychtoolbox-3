function UpdatePsychtoolbox()
% UpdatePsychtoolbox
%
% Updates the working copy of Psychtoolbox via the Subversion
% client from the Subversion master repository.
%
% This function will update your working copy of the Psychtoolbox with the
% latest bugfixes, enhancements and features from the Mastercopy.
%
%
% How to use:
%
% 1. Change your Matlab current directory to the Psychtoolbox - folder.
%
% 2. Call this script: 'UpdatePsychtoolbox'.
%
% 3. Your working copy will get updated to the latest release.
%
% 4. Enjoy the new version.

% History:
%
% 11/2/05 mk  Created.
% 1/13/06 mk  Added support for Microsoft Windows. 

% Windows system?
isWin = strcmp(computer, 'PCWIN');

% Check if subversion client is properly installed:
if ~isWin & exist('/usr/local/bin/svn', 'file')~=2
    fprintf('Could not find the Subversion client "svn" in its\n');
    fprintf('expected location /usr/local/bin/svn ! Please download\n');
    fprintf('and install the most recent Subversion client from...\n\n');
    fprintf('http://metissian.com/projects/macosx/subversion/\n\n');
    fprintf('... and then retry. Ciao.\n');
    return;
end;

fprintf('Trying to update working copy of OpenGL-Psychtoolbox.\n');

if ~isWin
    updatecommand = char([ '/usr/local/bin/svn update ./' ]);
else
    updatecommand = char([ 'svn update ./' ]);
end;
 
fprintf('Will execute the following update command, please standby...\n');
fprintf('%s\n', updatecommand);

if ~isWin
    [status, result] = system(updatecommand);
else
    [status, result] = dos(updatecommand);
end;
 
if (status>0)
    fprintf('Operation failed for some reason. Sorry...\n');
    fprintf('%s\n', result);
    return;   
end;

fprintf('Operation finished! The following list shows all modifications\n');
fprintf('applied to your copy of Psychtoolbox. Meaning of output is\n');
fprintf('U or G <filename>  -- File <filename> was modified/udpated.\n');
fprintf('A <filename>  -- New file <filename> was added.\n');
fprintf('D <filename>  -- File <filename> was removed.\n');
fprintf('C <filename>  -- File <filename> is in conflict with local changes!\n');
fprintf('If a file has state "C", this means something went wrong. Please check manually.\n');
fprintf('\n');
fprintf('List of modifications:\n');
fprintf('======================\n');
fprintf('%s\n', result);

fprintf('\n\n\nPlease delete and add the Psychtoolbox folder and its subfolders\n');
fprintf('to your Matlab path again to account for possible new subfolders...\n');
fprintf('After doing that, you will be ready to use the updated Psychtoolbox - Enjoy!\n\n')

return;
