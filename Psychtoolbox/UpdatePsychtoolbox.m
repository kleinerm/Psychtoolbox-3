function UpdatePsychtoolbox(targetdirectory, targetRevision)
% UpdatePsychtoolbox(targetdirectory, targetRevision)
%
% Update your working copy of the Psychtoolbox with the latest bug fixes,
% enhancements, and features from the master server.
%
% The "targetdirectory" argument is optional. If present, it gives the path
% of the Psychtoolbox folder to update. If omitted, UpdatePsychtoolbox will
% update the Psychtoolbox folder found by Matlab's or Octave's WHICH command. For
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
% 31.3.08 mk  Allow spaces in path to targetdirectory (Fix contributed by Tobias Wolf)
%  7.5.08 mk  Allow to spec 'targetdirectory' as [], so it gets default.
%
% 10/01/08 mk  Add interactive output/query for svn client on the Unices.
%              This to work-around questions of the client about accepting
%              security certificates...
% 01/05/09 mk  Remove && or || to make old Matlab versions happier.
% 05/31/09 mk  Add support for Octave-3.
% 10/05/09 mk  Strip trailing fileseperator from targetDirectory, as
%              suggested by Erik Flister to avoid trouble with new svn
%              clients.

% Flush all MEX files: This is needed at least on M$-Windows for SVN to
% work if Screen et al. are still loaded.
clear mex

if nargin < 1
    targetdirectory=[];
end

if isempty(targetdirectory)
    targetdirectory=PsychtoolboxRoot;
end

% Strip trailing fileseperator, if any:
if targetdirectory(end) == filesep
    targetdirectory = targetdirectory(1:end-1);
end

if nargin<2
    targetRevision = '';
else
    fprintf('Target revision: %s \n', targetRevision);
    targetRevision = [' -r ' targetRevision ' '];
end

fprintf('UpdatePsychtoolbox('' %s '') \n', targetdirectory);
fprintf('\n');

% Do notify user about potential trouble with path names with blanks in them:
if any(isspace(targetdirectory))
    fprintf('The targetdirectory spec contains white-space. This should work, but has not been tested extensively.\n');
end

% Check if this is a 64-bit Matlab, which we don't support at all:
if strcmp(computer,'PCWIN64') | strcmp(computer,'MACI64') | ...
  (~isempty(findstr(computer, '_64')) & isempty(findstr(computer, 'linux'))) %#ok<OR2>
    fprintf('Psychtoolbox does not work on a 64 bit version of Matlab or Octave.\n');
    fprintf('You need to install a 32 bit Matlab or Octave to install & use Psychtoolbox.\n');

    if strcmp(computer,'GLNXA64') | ~isempty(findstr(computer, '_64')) %#ok<OR2>
        fprintf('\nHowever, if you are a user of a Debian based GNU/Linux system, e.g.,\n');
        fprintf('Debian GNU/Linux or Ubuntu Linux, you can get a fully functional 64-bit\n');
        fprintf('version of Psychtoolbox very conveniently from the NeuroDebian project:\n');
        fprintf('http://neuro.debian.net\n\n');
    end
    
    error('Tried to update on a 64 bit version of Matlab or Octave, which is not supported.');
end

% Check OS
isWin=strcmp(computer,'PCWIN') | strcmp(computer,'PCWIN64') | strcmp(computer, 'i686-pc-mingw32');
isOSX=strcmp(computer,'MAC') | strcmp(computer,'MACI') | ~isempty(findstr(computer, 'apple-darwin'));
isLinux=strcmp(computer,'GLNX86') | strcmp(computer,'GLNXA64') | ~isempty(findstr(computer, 'linux-gnu'));

if ~isWin & ~isOSX & ~isLinux %#ok<AND2>
    os=computer;
    if strcmp(os,'MAC2')
        os='Mac OS9';
    end
    fprintf('Sorry, this updater doesn''t support your operating system: %s.\n',os);
    fprintf([mfilename ' can only install the new (OSX, Linux and Windows) \n'...
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
if isOSX & isempty(svnpath) %#ok<AND2>
    fprintf('The Subversion client "svn" is not in its expected\n');
    fprintf('location "/usr/local/bin/svn" on your disk. Please \n');
    fprintf('download and install the most recent Subversion client from:\n');
    fprintf('web http://metissian.com/projects/macosx/subversion/ -browser\n');
    fprintf('and then run %s again.\n',mfilename);
    error('Subversion client is missing.');
end

fprintf('About to update your working copy of the OpenGL-based Psychtoolbox.\n');
updatecommand=[svnpath 'svn update '  targetRevision ' ' strcat('"',targetdirectory,'"') ];
fprintf('Will execute the following update command:\n');
fprintf('%s\n', updatecommand);
if isOSX | isLinux %#ok<OR2>
    err=system(updatecommand);
    result = 'For reason, see output above.';
else
    [err, result]=dos(updatecommand, '-echo');
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
fprintf('Your MATLAB/OCTAVE path has been updated. Now trying to save the new MATLAB/OCTAVE path...\n\n');

% Does SAVEPATH work?
if exist('savepath') %#ok<EXIST>
   err=savepath;
else
   err=path2rc;
end

if err
    try
        % If this works then we're likely on Matlab:
        p=fullfile(matlabroot,'toolbox','local','pathdef.m');
        fprintf(['Sorry, SAVEPATH failed. Probably the pathdef.m file lacks write permission. \n'...
            'Please ask a user with administrator privileges to enable \n'...
            'write by everyone for the file:\n\n''%s''\n\n'],p);
    catch
        % Probably on Octave:
        fprintf(['Sorry, SAVEPATH failed. Probably your ~/.octaverc file lacks write permission. \n'...
            'Please ask a user with administrator privileges to enable \n'...
            'write by everyone for that file.\n\n']);
    end

    fprintf(['Once that''s done, run ' mfilename ' again. For this session, Psychtoolbox\n']);
    fprintf('will be fully functional, but you will need to save your path settings to make them persistent.\n\n');
end

fprintf('Fully done. Your new Psychtoolbox folder is ready to use. Enjoy!\n\n')

% Does a post-install routine exist? If so, we execute it, now that PTB is basically ready.
if exist('PsychtoolboxPostInstallRoutine.m', 'file')
   % We pass the information about downloaded flavor and that this is a download (=0) to the
   % post-install routine...
   clear PsychtoolboxPostInstallRoutine;
   PsychtoolboxPostInstallRoutine(1);
end;

return
