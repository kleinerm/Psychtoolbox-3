function SetupPsychtoolbox
% SetupPsychtoolbox - In-place setup of PTB without network access.
%
% This script prepares an already downloaded working copy of Psychtoolbox
% for use with Matlab. It sets proper Matlab paths, performs online
% registration if connected to a network and takes care of special setup
% operations for the Java based GetChar implementation.
%
% This setup routine is meant for people who want to install Psychtoolbox
% but don't have direct access to the internet. Installation in that case
% is a three step procedure:
%
% 1. Download and unpack a full working copy of PTB into your target
% folder. Obviously you need to somehow get a copy, either via conventional
% download from a computer with network connection (See 'help
% DownloadPsychtoolbox' or 'help UpdatePsychtoolbox') or from a helpful
% colleague.
%
% 2. Change your Matlab working directory to the Psychtoolbox installation
% folder, e.g., 'cd /Applications/Psychtoolbox'.
%
% 3. Type 'SetupPsychtoolbox' to run this script.
%
% Please be aware that the recommended method of installation is via the
% online Subversion system, i.e., DownloadPsychtoolbox and
% UpdatePsychtoolbox. Some functionality may not work with a copy that is
% set up via this script, e.g., PsychtoolboxVersion may provide incomplete
% version information. Convenient upgrades via UpdatePsychtoolbox may be
% impossible. Download size with this method is much higher as well.
%
% If you get stuck, post your question to the forum by email or web:
% web mailto:psychtoolbox@yahoogroups.com
% web http://groups.yahoo.com/group/psychtoolbox/messages/
% web http://groups.yahoo.com/group/psychtoolbox/post/
% Please specify your full name and the version of your operating system,
% MATLAB, and psychtoolbox.
%

% History:
%
% 01/25/07 mk  Created. Derived from DownloadPsychtoolbox. Basically a
%              stripped down version of it.

% Flush all MEX files: This is needed at least on M$-Windows to
% work if Screen et al. are still loaded.
clear mex

% Check OS
isWin=strcmp(computer,'PCWIN');
isOSX=strcmp(computer,'MAC') | strcmp(computer,'MACI');
if ~isWin & ~isOSX
os=computer;
if strcmp(os,'MAC2')
os='Mac OS9';
end
fprintf('Sorry, this installer doesn''t support your operating system: %s.\n',os);
fprintf([mfilename ' can only install the new (OSX and Windows) \n'...
   'OpenGL-based versions of the Psychtoolbox. To install the older (OS9 and Windows) \n'...
   'versions (not based on OpenGL) please go to the psychtoolbox website: \n'...
   'web http://psychtoolbox.org\n']);
error(['Your operating system is not supported by ' mfilename '.']);
end

% Locate ourselves:
targetdirectory=fileparts(which(fullfile('Psychtoolbox','SetupPsychtoolbox.m')));
if ~strcmpi(targetdirectory, pwd)
    error('You need to change your working directory to the Psychtoolbox folder before running this routine!');
end

fprintf('Will setup working copy of the Psychtoolbox folder inside: %s\n',targetdirectory);
fprintf('\n');
if any(isspace(targetdirectory))
    fprintf('Sorry. There cannot be any spaces in the target directory name:\n%s\n',targetdirectory);
    error('Cannot be any spaces in "targetdirectory" name.');
end

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
    fprintf(['Once "savepath" works (no error message), run ' mfilename ' again.\n']);
    fprintf('Alternatively you can choose to continue with installation, but then you will have\n');
    fprintf('to resolve this permission isssue later and add the path to the Psychtoolbox manually.\n');
    answer=input('Do you want to continue the installation despite the failure of SAVEPATH (yes or no)? ','s');
    if ~strcmp(answer,'yes')
        error('SAVEPATH failed. Please get an administrator to allow everyone to write pathdef.m.');
    end
end

% Remove "Psychtoolbox" from path
while any(regexp(path,[filesep 'Psychtoolbox[' filesep pathsep ']']))
    fprintf('Your old Psychtoolbox appears in the MATLAB path:\n');
    paths=regexp(path,['[^' pathsep ']*' pathsep],'match');
    fprintf('Your old Psychtoolbox appears %d times in the MATLAB path.\n',length(paths));
    answer=input('Before you decide to delete the paths, do you want to see them (yes or no)? ','s');
    if ~strcmp(answer,'yes')
        fprintf('You didn''t say "yes", so I''m taking it as no.\n');
    else
        for p=paths
            s=char(p);
            if any(regexp(s,[filesep 'Psychtoolbox[' filesep pathsep ']']))
                fprintf('%s\n',s);
            end
        end
    end
    answer=input('Shall I delete all those instances from the MATLAB path (yes or no)? ','s');
    if ~strcmp(answer,'yes')
        fprintf('You didn''t say yes, so I cannot proceed.\n');
        fprintf('Please use the MATLAB "File:Set Path" command to remove all instances of "Psychtoolbox" from the path.\n');
        error('Please remove Psychtoolbox from MATLAB path.');
    end
    for p=paths
        s=char(p);
        if any(regexp(s,[filesep 'Psychtoolbox[' filesep pathsep ']']))
            % fprintf('rmpath(''%s'')\n',s);
            rmpath(s);
        end
    end
    if exist('savepath')
       savepath;
    else
       path2rc;
    end

    fprintf('Success.\n\n');
end

% Add Psychtoolbox to MATLAB path
fprintf('Now adding the new Psychtoolbox folder (and all its subfolders) to your MATLAB path.\n');
p=targetdirectory;
pp=genpath(p);
addpath(pp);

if exist('savepath')
   err=savepath;
else
   err=path2rc;
end

if err
    fprintf('SAVEPATH failed. Psychtoolbox is now already installed and configured for use on your Computer,\n');
    fprintf('but i could not save the updated Matlab path, probably due to insufficient permissions.\n');
    fprintf('You will either need to fix this manually via use of the path-browser (Menu: File -> Set Path),\n');
    fprintf('or by manual invocation of the savepath command (See help savepath). The third option is, of course,\n');
    fprintf('to add the path to the Psychtoolbox folder and all of its subfolders whenever you restart Matlab.\n\n\n');
else 
    fprintf('Success.\n\n');
end

fprintf(['Now setting permissions to allow everyone to write to the Psychtoolbox folder. This will \n'...
    'allow future updates by every user on this machine without requiring administrator privileges.\n']);
if isOSX
    [s,m,mm]=fileattrib(p,'+w','a','s'); % recursively add write privileges for all users.
else
    [s,m,mm]=fileattrib(p,'+w','','s'); % recursively add write privileges for all users.
end
if s
    fprintf('Success.\n\n');
else
    fprintf('FILEATTRIB failed. Psychtoolbox will still work properly for you and other users, but only you\n');
    fprintf('or the system administrator will be able to run the UpdatePsychtoolbox script to update Psychtoolbox,\n');
    fprintf('unless you or the system administrator manually set proper write permissions on the Psychtoolbox folder.\n');
    fprintf('The error message of FILEATTRIB was: %s\n\n', m);
end

if exist('PsychtoolboxPostInstallRoutine.m', 'file')
   % Notify the post-install routine of the "pseudo-update" It will
   % determine the proper flavor by itself.
   PsychtoolboxPostInstallRoutine(1);
end

% Puuh, we are done :)
return
