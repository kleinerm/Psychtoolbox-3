function DownloadPsychtoolbox(flavor,targetdirectory)
% DownloadPsychtoolbox(flavor,targetdirectory)
%
% This script downloads the latest Mac OSX or Windows Psychtoolbox from the
% Subversion master server to your disk, creating your working copy, ready
% to use as a new toolbox in your MATLAB application. Subject to your
% permission, any old installation of the Psychtoolbox is first removed.
% It's a careful program, checking for all required resources and
% privileges before it starts.
% 
% The "targetdirectory" name may not contain any white space.
% 
% On OSX, your working copy of the Psychtoolbox will be placed in either
% your /Applications or your /Users/Shared folder (depending on permissions
% and your preference), or you may specify a 'targetdirectory', as you
% prefer. Unless you have a strong reason to do otherwise, we recommend
% that you accept the default, and put your Psychtoolbox in the
% Applications folder.
%
% On Microsoft Windows, you must specify the full path, including
% the drive name where Psychtoolbox should be installed, e.g.,
% 'C:\MyToolboxes\n'.
%
% You must specify the desired flavor of Psychtoolbox release via the
% "flavor" parameter: 'stable', 'beta' (aka 'current'), or
% 'Psychtoolbox-x.y.z'.
%
% 'stable' - Download the most recent fully tested release. This will
% become the next official Psychtoolbox release. It has been tested by
% developers and users in real world applications. Choose the 'stable'
% flavor if you don't like surprises. Stable releases are usually multiple
% months behind the Beta releases, so this is for the really anxious.
%
% 'beta', 'current' - Download the most recent beta release. This code has
% been tested by a few developers and is ready for testing by end users.
% Beta code that passes testing by end users will be promoted to the 'stable'
% branch. Choose the 'beta' flavor to get pre-release code with early access
% to bug fixes, enhancements, and new features.  The argument 'current' is
% a synonym for 'beta'.
%
% 'Psychtoolbox-x.y.z' - Download the code that corresponds to official
% release "Psychtoolbox-x.y.z", where "x.y.z" is the version number. This
% is nearly identical to the packages that can be downloaded from
% www.psychtoolbox.org. The difference is that critical bugs get fixed in
% this version.
%
% INSTALLATION INSTRUCTIONS:
%
% 1. If you don't already have it, you must install the Subversion client.
% For Mac OSX, download the latest Mac OSX Subversion client from:
% web http://metissian.com/projects/macosx/subversion/
% (You can ignore the Subversion README file. If you do read it, you can
% skip the instruction to manually add /usr/local/bin to your unix path.
% That's tricky to do, and not needed for installation and updates because
% we always specify the full path.)
%
% For Windows, download the Windows Subversion client from:
% web http://subversion.tigris.org/files/documents/15/34093/svn-1.4.0-setup.exe
% Install the Subversion client on your machine by double-clicking the
% installer and following the instructions. After installation of the
% Subversion client, you will need to exit and restart Matlab, so it
% can find the new subversion executable. In many cases it may be
% neccessary to even reboot your computer after installation of subversion.
%
% Alternatively, if you don't have the neccessary permissions to install
% Subversion into a system folder, you can install Subversion into an
% arbitrary folder on your system (excluding ones with blanks in their
% path) and then add that folder to your Matlab path. E.g. you installed
% into D:\MyOwnFolder\Subversion\ . Then you can do this in Matlab:
% addpath('D:\MyOwnFolder\Subversion\'). Our installer should find the
% client then.
%
% 2. We strongly recommend that you install the Psychtoolbox in the
% default location (/Applications or, failing that, /Users/Shared). Just
% type:
% DownloadPsychtoolbox('stable')
% or
% DownloadPsychtoolbox('beta') [or equivalently DownloadPsychtoolbox('current')]
% as you prefer. Our standard option is in the Applications folder, but note
% that, as with installation of any software, you'll need administrator privileges.
% Also note that if you put the toolbox in the Applications folder, you'll
% need to reinstall it when MATLAB is updated on your machine.
% If you must install without access to an administrator, we offer the
% option of installing into the /Users/Shared/ folder instead. If you must
% install the Psychtoolbox in some other folder, then specify it in the
% second argument of your call.
%
% That's it. Any pre-existing installation of the Psychtoolbox will be
% removed (if you approve). The program will then download the latest
% Psychtoolbox and update your MATLAB path. Enjoy! If you're new to this,
% you might start by typing "help Psychtoolbox".
%
% P.S. If you get stuck, post your question to the forum by email or web:
% web mailto:psychtoolbox@yahoogroups.com
% web http://groups.yahoo.com/group/psychtoolbox/messages/
% web http://groups.yahoo.com/group/psychtoolbox/post/
% Please specify your full name and the version of your operating system,
% MATLAB, and psychtoolbox.
%
% UPGRADE INSTRUCTIONS:
%
% To upgrade your copy of Psychtoolbox, at any time, to incorporate the
% latest bug fixes, enhancements, and new features, just type:
% UpdatePsychtoolbox
% 
% UpdatePsychtoolbox cannot change the flavor of your
% Psychtoolbox. To change the flavor, run DownloadPsychtoolbox.
% 
% PERMISSIONS:
%
% There's a thorny issue with permissions. It may not be possible to
% install into /Applications (or whatever the targetdirectory is) with the
% user's existing privileges. The normal situation on Mac OSX is that a few
% users have "administrator" privileges, and many don't. By default,
% writing to the /Applications folder requires administrator privileges.
% Thus all OSX installers routinely demand an extra authorization (if
% needed), asking the user to type in the name and password of an
% administrator before proceeding. We haven't yet figured out how to do
% that, but we want to offer that option. This conforms to normal
% installation of an application under Mac OS X.
%
% DownloadPsychtoolbox creates the Psychtoolbox folder with permissions set
% to allow writing by everyone. Our hope is that this will allow updating
% (by UpdatePsychtoolbox) without need for administrator privileges.
%
% Some labs that may want to be able to install without access to an
% administrator. For them we offer the fall back of installing Psychtoolbox
% in /Users/Shared/, instead of /Applications/, because, by default,
% /Users/Shared/ is writeable by all users.
%
% SAVEPATH
%
% Normally all users of MATLAB use the same path. This path is normally
% saved in MATLABROOT/toolbox/local/pathdef.m, where "MATLABROOT" stands
% for the result returned by running that function in MATLAB, e.g.
% '/Applications/MATLAB.app/Contents/Matlab14.1'. Since pathdef.m is inside
% the MATLAB package, which is normally in the Applications folder,
% ordinary users (not administrators) cannot write to pathdef.m. They'll
% get an error message whenever they try to save the path, e.g. by typing
% "savepath". Most users will find this an unacceptable limitation. The
% solution is very simple, ask an administrator to use File Get Info to set
% the pathdef.m file permissions to allow write by everyone. This needs to
% be done only once, after installing MATLAB.
% web http://www.mathworks.com/access/helpdesk/help/techdoc/matlab_env/ws_pat18.html

% History:
%
% 11/02/05 mk  Created.
% 11/25/05 mk  Bug fix for 'targetdirectory' provided by David Fencsik.
% 01/13/06 mk  Added support for download of Windows OpenGL-PTB.
% 03/10/06 dgp Expanded the help text, above, incorporating suggestions
%              from Daniel Shima.
% 03/11/06 dgp Check OS. Remove old Psychtoolbox from path and from disk.
%              After downloading, add new Psychtoolbox to path.
% 03/12/06 dgp Polished error message regarding missing svn.
% 03/13/06 dgp Changed default targetdirectory from PWD to ~/Documents/.
% 03/14/06 dgp Changed default targetdirectory to /Applications/, and
%              if not sufficiently privileged, then /Users/Shared/.
%              Check privilege to create folder. Check SAVEPATH.
% 06/05/06 mk  On Windows, we require the user to pass the full path to
%              the installation folder, because there is no well-defined
%              equivalent to the Mac OS X /Applications/ folder. Also, the order
%              of operations was changed to maximize the chance of getting a
%              working PTB installation despite minor failures of commands
%              like savepath, or fileattrib. We allow the user to decide
%              whether to delete her old Psychtoolbox folders or to
%              retain multiple copies of Psychtoolbox (e.g., beta and stable)
%              so each user can choose between Beta and Stable.
%              We no longer copy UpdatePsychtoolbox.m, since it's included in the 
%              new Psychtoolbox folder 
% 06/06/06 dgp Cosmetic editing of comments, above.
% 06/27/06 dgp Cosmetic editing of comments and messages. Check for spaces
%              in targetdirectory name.
% 9/23/06  mk  Add clear mex call to flush mex files before downloading.
% 10/5/06  mk  Add detection code for MacOS-X on Intel Macs.
% 10/28/06 dhb Allow 'current' as a synonym for 'beta'.
% 11/21/06 mk  Allow alternate install location for svn client: Installer
%              will find the svn executable if its installation folder is
%              included in the Matlab path.
% 02/17/07 mk  Convert flavor spec to lower case in case it isn't.
% 03/15/07 mk  Detection code for Windows 64 bit added.

% Flush all MEX files: This is needed at least on M$-Windows for SVN to
% work if Screen et al. are still loaded.
clear mex

% Check OS
isWin=strcmp(computer,'PCWIN') | strcmp(computer,'PCWIN64');
isOSX=strcmp(computer,'MAC') | strcmp(computer,'MACI');
if ~isWin && ~isOSX
os=computer;
if strcmp(os,'MAC2')
os='Mac OS9';
end
fprintf('Sorry, this installer doesn''t support your operating system: %s.\n',os);
fprintf([mfilename ' can only install the new (OSX and Windows) \n'...
   'OpenGL-based versions of the Psychtoolbox. To install the older (OS9 and Windows) \n'...
   'versions (not based on OpenGL) please go to the psychtoolbox website: \n'...
   'web http://psychtoolbox.org/download.html\n']);
error(['Your operating system is not supported by ' mfilename '.']);
end

if nargin<2
    if isOSX
        % Set default path for OSX install:
        targetdirectory=fullfile(filesep,'Applications');
    else
        % We do not have a default path on Windows, so the user must provide it:
        fprintf('You did not provide the full path to the directory where Psychtoolbox should be\n');
        fprintf('installed. This is required for Microsoft Windows installation. Please enter a full\n');
        fprintf('path as the second argument to this script, e.g. DownloadPsychtoolbox(''beta'',''C:\\Toolboxes\\'').\n');
        error('For Windows, the call to %s must specify a full path for the location of installation.',mfilename);
    end     
end

% Set flavor defaults and synonyms
if (nargin<1 | isempty(flavor))
    flavor='stable';
end

% Make sure that flavor is lower-case:
flavor = lower(flavor);

switch (flavor)
    % 'current' is a synonym for 'beta'.
    case 'current'
        flavor = 'beta';
end

fprintf('DownloadPsychtoolbox(''%s'',''%s'')\n',flavor,targetdirectory);
fprintf('Requested flavor is: %s\n',flavor);
fprintf('Requested location for the Psychtoolbox folder is inside: %s\n',targetdirectory);
fprintf('\n');
if any(isspace(targetdirectory))
    fprintf('Sorry. There cannot be any spaces in the target directory name:\n%s\n',targetdirectory);
    error('Cannot be any spaces in "targetdirectory" name.');
end

% Check for alternative install location of Subversion:
if isWin
    % Search for Windows executable in Matlabs path:
    svnpath = which('svn.exe');
else
    % Search for Unix executable in Matlabs path:
    svnpath = which('svn.');
end

% Found one?
if ~isempty(svnpath)
    % Extract basepath and use it:
    svnpath=[fileparts(svnpath) filesep];
else
    % Could not find svn executable in Matlabs path. Check the default
    % install location on OS-X and abort if it isn't there. On M$-Win we
    % simply have to hope that it is in some system dependent search path.

    % Currently, we only know how to check this for Mac OSX.
    if isOSX
        svnpath='/usr/local/bin/';
        if exist('/usr/local/bin/svn','file')~=2
            fprintf('The Subversion client "svn" is not in its expected\n');
            fprintf('location "/usr/local/bin/svn" on your disk. Please \n');
            fprintf('download and install the most recent Subversion client from:\n');
            fprintf('web http://metissian.com/projects/macosx/subversion/ -browser\n');
            fprintf('and then run %s again.\n',mfilename);
            error('Subversion client is missing. Please install it.');
        end
    end
end

if ~isempty(svnpath)
    fprintf('Will use the svn client which is located in this folder: %s\n', svnpath);
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

% Do we have sufficient privileges to install at the requested location?
p='Psychtoolbox123test';
[success,m,mm]=mkdir(targetdirectory,p);
if success
    rmdir(fullfile(targetdirectory,p));
else
    if strcmp(m,'Permission denied')
        if isOSX
            fprintf([
            'Sorry. You would need administrator privileges to install the \n'...
            'Psychtoolbox into the ''%s'' folder. You can either quit now \n'...
            '(say "no", below) and get a user with administrator privileges to run \n'...
            'DownloadPsychtoolbox for you, or you can install now into the \n'...
            '/Users/Shared/ folder, which doesn''t require special privileges. We \n'...
            'recommend installing into the /Applications/ folder, because it''s the \n'...
            'normal place to store programs. \n\n'],targetdirectory);
            answer=input('Even so, do you want to install the Psychtoolbox into the \n/Users/Shared/ folder (yes or no)? ','s');
            if ~strcmp(answer,'yes')
                fprintf('You didn''t say yes, so I cannot proceed.\n');
                error('Need administrator privileges for requested installation into folder: %s.',targetdirectory);
            end
            targetdirectory='/Users/Shared';
        else
            % Windows: We simply fail in this case:
            fprintf([
            'Sorry. You would need administrator privileges to install the \n'...
            'Psychtoolbox into the ''%s'' folder. Please rerun the script, choosing \n'...
            'a location where you have write permission, or ask a user with administrator \n'...
            'privileges to run DownloadPsychtoolbox for you.\n\n'],targetdirectory);
            error('Need administrator privileges for requested installation into folder: %s.',targetdirectory);
        end
    else
        error(mm,m);
    end
end
fprintf('Good. Your privileges suffice for the requested installation into folder %s.\n\n',targetdirectory);

% Delete old Psychtoolbox
skipdelete = 0;
while (exist('Psychtoolbox','dir') || exist(fullfile(targetdirectory,'Psychtoolbox'),'dir')) && (skipdelete == 0)
    fprintf('Hmm. You already have an old Psychtoolbox folder:\n');
    p=fullfile(targetdirectory,'Psychtoolbox');
    if ~exist(p,'dir')
        p=fileparts(which(fullfile('Psychtoolbox','Contents.m')));
        if length(p)==0
            w=what('Psychtoolbox');
            p=w(1).path;
        end
    end
    fprintf('%s\n',p);
    fprintf('That old Psychtoolbox should be removed before we install a new one.\n');
    if ~exist(fullfile(p,'Contents.m'))
        fprintf(['WARNING: Your old Psychtoolbox folder lacks a Contents.m file. \n'...
            'Maybe it contains stuff you want to keep. Here''s a DIR:\n']);
        dir(p)
    end

    fprintf('First we remove all references to "Psychtoolbox" from the MATLAB path.\n');
    pp=genpath(p);
    warning('off','MATLAB:rmpath:DirNotFound');
    rmpath(pp);
    warning('on','MATLAB:rmpath:DirNotFound');
    
    if exist('savepath')
       savepath;
    else
       path2rc;
    end

    fprintf('Success.\n');

    s=input('Shall I delete the old Psychtoolbox folder and all its contents \n(recommended in most cases), (yes or no)? ','s');
    if strcmp(s,'yes')
        skipdelete = 0;
        fprintf('Now we delete "Psychtoolbox" itself.\n');
        [success,m,mm]=rmdir(p,'s');
        if success
            fprintf('Success.\n\n');
        else
            fprintf('Error in RMDIR: %s\n',m);
            fprintf('If you want, you can delete the Psychtoolbox folder manually and rerun this script to recover.\n');
            error(mm,m);
        end
    else
        skipdelete = 1;
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

% Download Psychtoolbox
if isOSX
    fprintf('I will now download the latest Psychtoolbox for OSX.\n');
else
    fprintf('I will now download the latest Psychtoolbox for Windows.\n');
end
fprintf('Requested flavor is: %s\n',flavor);
fprintf('Target folder for installation: %s\n',targetdirectory);
p=fullfile(targetdirectory,'Psychtoolbox');
if any(isspace(p)) % Double check, to be sure.
    error('No spaces are allowed in the destination folder name.');
end

checkoutcommand=[svnpath 'svn checkout svn://svn.berlios.de/osxptb/' flavor '/Psychtoolbox/ ' p];
fprintf('The following CHECKOUT command asks the Subversion client to \ndownload the Psychtoolbox:\n');
fprintf('%s\n',checkoutcommand);
fprintf('Downloading. It''s nearly 100 MB, which can take many minutes. \nAlas there is no output to this window to indicate progress until the download is complete. \nPlease be patient ...\n');
if isOSX
    [err,result]=system(checkoutcommand);
else
    [err,result]=dos(checkoutcommand);
end

if err
    % Failed! Let's retry it via http protocol. This may work-around overly
    % restrictive firewalls or otherwise screwed network proxies:
    fprintf('Command "CHECKOUT" failed with error code %d: \n',err);
    fprintf('%s\n\n',result);
    fprintf('Will retry now by use of alternative http protocol...\n');
    checkoutcommand=[svnpath 'svn checkout http://svn.berlios.de/svnroot/repos/osxptb/' flavor '/Psychtoolbox/ ' p];
    fprintf('The following alternative CHECKOUT command asks the Subversion client to \ndownload the Psychtoolbox:\n');
    fprintf('%s\n\n',checkoutcommand);
    if isOSX
        [err,result]=system(checkoutcommand);
    else
        [err,result]=dos(checkoutcommand);
    end    
end

if err
    fprintf('Sorry, the download command "CHECKOUT" failed with error code %d: \n',err);
    fprintf('%s\n',result);
    fprintf('The download failure might be due to temporary network or server problems. You may want to try again in a\n');
    fprintf('few minutes. It could also be that the subversion client was not (properly) installed. On Microsoft\n');
    fprintf('Windows you will need to exit and restart Matlab after installation of the Subversion client. If that\n');
    fprintf('does not help, you will need to reboot your machine before proceeding.\n');
    error('Download failed.');
end
fprintf('Download succeeded!\n\n');

% Add Psychtoolbox to MATLAB path
fprintf('Now adding the new Psychtoolbox folder (and all its subfolders) to your MATLAB path.\n');
p=fullfile(targetdirectory,'Psychtoolbox');
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

fprintf('You can now use your newly installed ''%s''-flavor Psychtoolbox. Enjoy!\n',flavor);
fprintf('Whenever you want to upgrade your Psychtoolbox to the latest ''%s'' version, just\n',flavor);
fprintf('run the UpdatePsychtoolbox script.\n\n');

if exist('PsychtoolboxPostInstallRoutine.m', 'file')
   % Notify the post-install routine of the download and its flavor.
   PsychtoolboxPostInstallRoutine(0, flavor);
end

% Puuh, we are done :)
return
