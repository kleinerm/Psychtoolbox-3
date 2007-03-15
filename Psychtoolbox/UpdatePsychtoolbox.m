function UpdatePsychtoolbox(targetdirectory, targetRevision)
% UpdatePsychtoolbox(targetdirectory, targetRevision)
%
% Update your working copy of the Psychtoolbox with the latest bug fixes,
% enhancements, and features from the master server.
%
% The "targetdirectory" argument is optional. If present, it gives the path
% of the Psychtoolbox folder to update. If omitted, UpdatePsychtoolbox will
% update the Psychtoolbox folder found by MATLAB's WHICH command. For
% example:
% UpdatePsychtoolbox
% UpdatePsychtoolbox('~/Applications/Psychtoobox')
%
% The "targetRevision" argument is optional and should be normally omitted.
% Normal behaviour is to upgrade your working copy to the latest revision.
% If you provide a specific targetRevision, then this script will
% *downgrade* your copy of Psychtoolbox to the specified revision.
% This is only useful if you experience problems after an update and want
% to revert to an earlier known-to-be-good release.
% Revisions can be specified by a revision number, a specific date, or by
% the special flag 'PREV' which will downgrade to the revision before the
% most current one. By executing this script multiple times with the 'PREV'
% specifier, you can incrementally downgrade until stuff works for you.
%
% UpdatePsychtoolbox cannot change the beta-vs-stable flavor of your
% Psychtoolbox. To change the flavor, run DownloadPsychtoolbox again.

% History:
%
% 11/2/05 mk  Created.
% 1/13/06 mk  Added support for Microsoft Windows.
% 3/13/06 dgp Allow explicit targetdirectory argument.
%             Changed default to use WHICH instead of PWD as
%             targetdirectory. Update MATLAB path. Expand comments.
% 5/08/06 mk  Small fixes. Added option to downgrade to a specific revision.
% 9/23/06 mk  Add clear mex call to flush mex files before updating.
% 10/5/06 mk  Add detection code for MacOS-X on Intel Macs.
% 21/11/06 mk Add improved detection code for Subversion.

% Flush all MEX files: This is needed at least on M$-Windows for SVN to
% work if Screen et al. are still loaded.
clear mex

if nargin<1
    targetdirectory=fileparts(which(fullfile('Psychtoolbox','Contents.m')));
end

if nargin<2
    targetRevision = '';
else
    fprintf('Target revision: %s \n', targetRevision);
    targetRevision = [' -r ' targetRevision ' '];
end

fprintf('UpdatePsychtoolbox('' %s '') \n', targetdirectory);
fprintf('\n');

% Do not accept path names with blanks in them:
if any(isspace(targetdirectory))
    error('The targetdirectory spec contains white-space. This is not allowed!');
end

% Check OS
isWin=strcmp(computer,'PCWIN') | strcmp(computer,'PCWIN64');
isOSX=strcmp(computer,'MAC') | strcmp(computer,'MACI');
if ~isWin && ~isOSX
    os=computer;
    if strcmp(os,'MAC2')
        os='Mac OS9';
    end
    fprintf('Sorry, this updater doesn''t support your operating system: %s.\n',os);
    fprintf([mfilename ' can only install the new (OSX and Windows) \n'...
        'OpenGL-based versions of the Psychtoolbox. To install the older (OS9 and Windows) \n'...
        'versions (not based on OpenGL) please go to the psychtoolbox website: \n'...
        'web http://psychtoolbox.org/download.html\n']);
    error(['Your operating system is not supported by ' mfilename '.']);
end

% Save old Psychtoolbox path
oldPath = RemoveSVNPaths(genpath(targetdirectory));

% Retrieve path to Subversion executable:
svnpath = GetSubversionPath;

% Check that subversion client is installed.
% Currently, we only know how to check this for Mac OSX.
if isOSX && isempty(svnpath)
    fprintf('The Subversion client "svn" is not in its expected\n');
    fprintf('location "/usr/local/bin/svn" on your disk. Please \n');
    fprintf('download and install the most recent Subversion client from:\n');
    fprintf('web http://metissian.com/projects/macosx/subversion/ -browser\n');
    fprintf('and then run %s again.\n',mfilename);
    error('Subversion client is missing.');
end

fprintf('About to update your working copy of the OpenGL-based Psychtoolbox.\n');
updatecommand=[svnpath 'svn update '  targetRevision targetdirectory ];
fprintf('Will execute the following update command:\n');
fprintf('%s\n', updatecommand);
if isOSX
    [err, result]=system(updatecommand);
else
    [err, result]=dos(updatecommand);
end
if err
    fprintf('Sorry. The update command failed:\n');
    fprintf('%s\n', result);
    error('Update failed.');
end
fprintf('Success!\n\n');
fprintf('CHANGES:\n');
fprintf('%s\n',result);
fprintf('CHANGE             -- MEANING\n');
fprintf('U or G <filename>  -- File <filename> was modified/updated.\n');
fprintf('A <filename>       -- New file <filename> was added.\n');
fprintf('D <filename>       -- File <filename> was removed.\n');
fprintf('C <filename>       -- File <filename> is in conflict with local changes!\n');
fprintf('"C" indicates that something went wrong. Please check manually.\n');
fprintf('A conflict happens if you manually modified files in the Psychtoolbox folder in\n');
fprintf('a way that conflicts with the new file from the update and if that conflict can\n');
fprintf('not get automatically resolved.\n');
fprintf('If you can not resolve such a conflict, the simplest solution is to manually\n');
fprintf('delete the file or subfolder for which a conflict is reported, and then run\n');
fprintf('UpdatePsychtoolbox again. It will download and add the proper missing files.\n');
fprintf('If everything else fails, simply delete the whole Psychtoolbox folder and use\n');
fprintf('DownloadPsychtoolbox again for a full, clean download.\n');
fprintf('\n');

% Remove old Psychtoolbox paths. Add new Psychtoolbox paths.
rmpath(oldPath);
addpath(genpath(targetdirectory));
fprintf('Your MATLAB path has been updated. Now trying to save the new MATLAB path...\n\n');

% Does SAVEPATH work?
if exist('savepath')
   err=savepath;
else
   err=path2rc;
end

if err
p=fullfile(matlabroot,'toolbox','local','pathdef.m');
fprintf(['Sorry, SAVEPATH failed. Probably the pathdef.m file lacks write permission. \n'...
        'Please ask a user with administrator privileges to enable \n'...
        'write by everyone for the file:\n''%s''\n'],p);
fprintf(['Once that''s done, run ' mfilename ' again. For this MATLAB session, Psychtoolbox\n']);
fprintf('will be fully functional, but you will need to save your path settings to make them persistent.\n\n');
error('SAVEPATH failed. Please get an administrator to allow everyone to write pathdef.m.');
end

fprintf('Fully done. Your new Psychtoolbox folder is ready to use. Enjoy!\n\n')

% Does a post-install routine exist? If so, we execute it, now that PTB is basically ready.
if exist('PsychtoolboxPostInstallRoutine.m', 'file')
   % We pass the information about downloaded flavor and that this is a download (=0) to the
   % post-install routine...
   PsychtoolboxPostInstallRoutine(1);
end;

return
