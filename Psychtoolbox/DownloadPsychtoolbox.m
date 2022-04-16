function DownloadPsychtoolbox(targetdirectory, flavor, targetRevision, downloadmethod, tryNonInteractiveSetup)
% DownloadPsychtoolbox([targetdirectory][, flavor][, targetRevision][, downloadmethod][, tryNonInteractiveSetup=0])
%
% This script downloads the latest GNU/Linux, MS-Windows, or Apple macOS
% Psychtoolbox-3, version 3.0.10 or later, from our git-server to your
% disk, creating your working copy, ready to use as a new toolbox in your
% MATLAB/OCTAVE application. Subject to your permission, any old
% installation of the Psychtoolbox is first removed. It's a careful
% program, checking for all required resources and privileges before it
% starts.
%
% Note: If you use a Debian derived Linux distribution, e.g., Debian or
% Ubuntu, consider installing the package octave-psychtoolbox-3 or
% matlab-psychtoolbox-3 instead from http://neuro.debian.net - This is more
% convenient and will provide you with automatic updates.
%
% CAUTION: Psychtoolbox 3.0.14 will not work anymore with OSX versions
% earlier than 10.11 "El Capitan".
%
% CAUTION: Psychtoolbox 3.0.13 will not work anymore with 32-Bit Octave-4
% on MS-Windows, or with OSX versions earlier than 10.10 "Yosemite".
% Psychtoolbox will likely work with versions of Microsoft Windows older
% than Windows-10, but it is not tested on such systems anymore and not
% supported at all on Windows versions earlier than Windows-7. For best
% compatibility you should probably use Windows-10 with all upgrades
% installed.
%
% CAUTION: Psychtoolbox 3.0.12 will not work anymore with 32-Bit Matlab, or
% with OSX versions earlier than 10.8 "Mountain Lion". Psychtoolbox may
% work with versions of Microsoft Windows older than Windows-7, but it is
% not tested or supported on such ancient Windows systems anymore, so use
% at your own risk.
%
% Psychtoolbox 3.0.11 *will not work* with GNU/Octave on MS-Windows, or
% with 32-Bit Octave on OSX, as support for these setups has been cancelled
% for the 3.0.10 series. It will also not work with 32-Bit Matlab on OSX,
% or with OSX versions earlier than 10.6.8 "Snow Leopard", unless you
% choose the unsupported legacy flavor "Psychtoolbox-3.0.10" via the
% optional 'flavor' parameter.
%
% If you want to download older versions of Psychtoolbox than 3.0.10, e.g.,
% version 3.0.9, use the DownloadLegacyPsychtoolbox() function instead of
% this function.
%
% On Mac OSX, all parameters are optional. On MS-Windows and GNU/Linux, the
% first parameter "targetdirectory" with the path to the installation
% target directory is required. The "targetdirectory" name may not contain
% any white space, otherwise download may fail with mysterious error
% messages!
% 
% On OSX, your working copy of the Psychtoolbox will be placed in either
% your /Applications or your /Users/Shared folder (depending on permissions
% and your preference), or you may specify a 'targetdirectory', as you
% prefer.
%
% On Microsoft Windows, you must specify the full path, including
% the drive name where Psychtoolbox should be installed, e.g.,
% 'C:\MyToolboxes\'.
%
% The desired flavor of a Psychtoolbox release can be selected via the
% optional "flavor" parameter: By default, 'beta' (aka 'current') will be
% installed if you don't specify otherwise, as this is almost always the
% best possible choice. You may be able to download an old versioned
% release via a namestring like 'Psychtoolbox-x.y.z', e.g.,
% 'Psychtoolbox-3.0.14' if you'd want to download version 3.0.14. This is
% only useful if you run a very old operating system or Matlab version that
% isn't supported by the current "beta" anymore, so you'd need to stick
% with an old versioned release.
%
% Normally your download should just work(TM). Very infrequently, the
% download servers may be overloaded or down for maintenance, resulting in
% download failure. In that case, please retry a few hours later.
%
%
% The "targetRevision" argument is optional and should be normally omitted.
% Normal behaviour is to download the latest revision of Psychtoolbox. If
% you provide a specific targetRevision, e.g., 1234 for revision 1234, then
% this script will install a copy of Psychtoolbox according to the
% specified revision.
%
% This is only useful if you experience problems and want to revert to an
% earlier known-to-be-good release.
%
% Revisions can be specified by a revision number or by the special flag
% 'PREV' which will choose the revision before the most current one.
%
%
% 'downloadmethod' Optional: The method to use or prefer for download. By
% default method 0 is used, which means to perform a Subversion checkout of
% a fully versioned working copy, including all revision information, which
% allows to go backward in time or update efficiently via the
% UpdatePsychtoolbox() command, query the current version and revision of
% Psychtoolbox via PsychtoolboxVersion() command, and access the
% development history via svn command-line tools etc. A setting of 1
% requests an export without versioning information, essentialy a dumb
% download. This will cut download size and disc space usage in less than
% half, but you won't be able to use UpdatePsychtoolbox() or
% PsychtoolboxVersion() or other svn features anymore.
%
% By default, the most efficient and convenient method of Subversion
% download is used, e.g., using Matlabs integrated SVNKit for Matlab R2014b
% or later, instead of a svn command-line client. You can always enforce
% use of an installed svn command-line client via setting downloadmethod to
% -1. This is the old way of doing things, in case the improved method via
% SVNKit should not work for some reason.
%
%
% The optional parameter 'tryNonInteractiveSetup' if provided as 1 (true), will
% try a setup without user interaction, not asking users for input in certain
% situations, but assuming an answer that keeps the setup progressing. Note that
% this is not guaranteed to work in all cases, and may end in data loss, e.g.,
% overwriting an old and potentially user-modified Psychtoolbox installation.
% This non-interactice setup mode is highly experimental, not well tested, not
% supported in case of any trouble!
%
%
% INSTALLATION INSTRUCTIONS: The Wiki contains much more up to date
% instructions. If in doubt, follow instructions on the Wiki!
%
% 0. If you are using Matlab R2014b or later, skip to step 2.
%
%
% 1. On older Matlab versions, or on Octave, if you don't already have it,
% you must install a Subversion client.
%
%
% On Linux, use the OS package manager, e.g., on Debian/Ubuntu based
% systems:
%
% sudo apt install subversion
%
%
% On Windows, download the Windows Subversion client from the internet, e.g.,
% from one of these:
%
% http://subversion.apache.org/packages.html#windows
% http://www.wandisco.com/subversion/download#windows
%
%
% On macOS, if you happen to have HomeBrew (http://brew.sh) installed, a
%
% brew install subversion
%
% should do the trick.
%
%
% After installation of the Subversion client, you will need to exit and
% restart Matlab or Octave, so it can find the new subversion executable.
%
% Btw. you should avoid to install the client into a path that contains
% blanks/spaces/white-space as this can lead to download failures in some
% cases, e.g., 'C:\Program Files\...' may be bad because there is a blank
% between the "Program" and "Files".
%
% Alternatively, if you don't have the neccessary permissions to install
% Subversion into a system folder, you can install Subversion into an
% arbitrary folder on your system (excluding ones with blanks in their
% path) and then add that folder to your Matlab or Octave path. E.g. you
% installed into D:\MyOwnFolder\Subversion\ . Then you can do this:
% addpath('D:\MyOwnFolder\Subversion\'). Our installer should find the
% client then.
%
%
% 2. On macOS, to install the Psychtoolbox in the default location
% (/Applications or, failing that, /Users/Shared). Just type:
%
% DownloadPsychtoolbox
%
% Our standard option is in the Applications folder, but note that, as with
% installation of any software, you'll need administrator privileges. Also
% note that if you put the toolbox in the Applications folder, you'll need
% to reinstall it when MATLAB / OCTAVE is updated on your machine. If you
% must install without access to an administrator, we offer the option of
% installing into the /Users/Shared/ folder instead. If you want to install
% the Psychtoolbox in some other folder, then specify it in the optional
% first argument of your call.
%
% On Windows or Linux, provide a pathname, e.g.:
% DownloadPsychtoolbox('C:\MyToolboxes\');
%
% That's it. Any pre-existing installation of the Psychtoolbox will be
% removed (if you approve). The program will then download the latest
% Psychtoolbox and update your MATLAB / OCTAVE path and other relevant
% system settings.
%
% Enjoy! If you're new to this, you might start by typing "help
% Psychtoolbox".
%
% P.S. If you get stuck, first check the FAQ section and Download section
% of our Wiki at http://www.psychtoolbox.org. If that doesn't help, post
% your question to the forum:
%
% https://psychtoolbox.discourse.group
%
% Please specify your full name and the version of your operating system,
% MATLAB / OCTAVE, and psychtoolbox.
%
% UPGRADE INSTRUCTIONS:
%
% To upgrade your copy of Psychtoolbox, at any time, to incorporate the
% latest bug fixes, enhancements, and new features, just type:
% UpdatePsychtoolbox
% 
% UpdatePsychtoolbox cannot change the flavor of your Psychtoolbox. To
% change the flavor, run DownloadPsychtoolbox to completely discard your
% old installation and get a fresh copy with the requested flavor. If you
% used DownloadPsychtoolbox with the optional 'downloadmethod' set to 1
% then UpdatePsychtoolbox() won't work. That's the price you pay for saving
% disc space.
% 
% PERMISSIONS:
%
% There's a thorny issue with permissions on OS/X. It may not be possible
% to install into /Applications (or whatever the targetdirectory is) with
% the user's existing privileges. The normal situation on Mac OSX is that a
% few users have "administrator" privileges, and many don't. By default,
% writing to the /Applications folder requires administrator privileges.
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
% Normally all users of MATLAB / OCTAVE use the same path. This path is
% normally saved in MATLABROOT/toolbox/local/pathdef.m, where "MATLABROOT"
% stands for the result returned by running that function in MATLAB, e.g.
% '/Applications/MATLAB.app/Contents/Matlab14.1'. Since pathdef.m is inside
% the MATLAB package, which is normally in the Applications folder,
% ordinary users (not administrators) cannot write to pathdef.m. They'll
% get an error message whenever they try to save the path, e.g. by typing
% "savepath". Most users will find this an unacceptable limitation. The
% solution is very simple, ask an administrator to use File Get Info to set
% the pathdef.m file permissions to allow write by everyone. This needs to
% be done only once, after installing MATLAB.
%
% web http://www.mathworks.com/access/helpdesk/help/techdoc/matlab_env/ws_pat18.html
%
%

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
% 07/18/07 mk  Changed default for flavor from 'stable' to 'beta'.
% 09/27/07 mk  Add another fallback path: Download via https protocol to
%              maybe bypass proxy-servers.
% 10/29/07 mk  Small fix for Kerstin Preuschoffs bugreport: Download of old
%              versions didn't work anymore, becuase 'flavor' string was
%              lower-cased.
% 11/17/07 mk  Prioritized https:// checkout over http:// checkout --> More
%              likely to bypass proxy servers and succeed.
% 12/25/07 mk  Add check for white spaces in path to subversion client - Output warning, if so.
% 12/25/07 mk  Add optional 'downloadmethod' parameter, which allows to
%              select initial choie of download protocol to use, in order to allow to
%              bypass misconfigured proxies and firewalls. Problem reported
%              by Patrick Mineault.
% 01/08/08 mk  On OS/X, add an additional search path for 'svn': /usr/bin/
%              as per suggestion of Donald Kalar. Presumably, Apples Leopard ships
%              with a svn client preinstalled in that location.
%
% 05/07/08 mk  Add better handling of default values. Add 'targetRevision'
%              parameter as option, just as in UpdatePsychtoolbox.m
%
% 06/16/08 mk  Change default initial download protcol from svn: to https:,
%              as Berlios seems to have disabled svnserve protocol :-(
%
% 10/01/08 mk  Add interactive output/query for svn client on the Unices.
%              This to work-around questions of the client about accepting
%              security certificates...
% 01/05/09 mk  Remove && or || to make old Matlab versions happier.
% 01/05/09 mk  Remove && or || to make old Matlab versions happier.
% 01/05/09 mk  Change order of call arguments to make choice of default
%              'beta' flavor more convenient and choice of 'stable'
%              inconvenient. Also add some additional warning text and
%              dialog to tell user how bad 'stable' is and to give him a
%              chance to reconsider.
% 03/22/09 mk  Update help text again. Rename 'stable' into 'unsupported'.
% 05/31/09 mk  Add support for Octave-3.
% 10/05/09 mk  Strip trailing fileseperator from targetDirectory, as
%              suggested by Erik Flister to avoid trouble with new svn
%              clients.
% 12/27/10 mk  Redirect 'beta' downloads on Matlab versions < 6.5 to the
%              special "Psychtoolbox-3.0.8-PreMatlab6.5" compatibility
%              version - The last one to support pre 6.5 Matlab's.
%
% 10/31/11 mk  Change location of SVN repository to
%              http://psychtoolbox-3.googlecode.com/svn/
%              our new home, now that Berlios is shutting down.
%
% 05/27/12 mk  - Strip backwards compatibility support to Matlab pre-R2007a.
%              - Remove PowerPC support.
%              - Strip support for 'stable' / 'unsupported' flavors et al.
%              - Change location of SVN repository to our SVN frontend for GIT:
%                https://github.com/Psychtoolbox-3/Psychtoolbox-3
%
% 05/28/12 mk  Fix disasterous bug: Answering anything but 'y' to the
%              question if old PTB folder should be deleted, caused
%              *deletion* of the folder! Oh dear! This bug present since
%              late 2011.
%
% 09/14/12 mk  Drop support for Octave on MS-Windows.
% 09/14/12 mk  Drop support for 32-Bit Octave on OSX.
% 03/10/13 mk  Add additional svn search-pathes matching UpdatePsychtoolbox et al.
%              Also update download URL for OSX Suversion client.
% 07/02/13 mk  Drop support for 32-Bit Matlab on OSX, and thereby for 32-Bit OSX.
% 07/02/13 mk  Reenable write access for all to Psychtoolbox folder.
% 07/23/13 mk  Do not prevent execution on 32-Bit Matlab on OSX!
% 05/18/14 mk  No support for 32-Bit Matlab on Linux and Windows anymore for 3.0.12.
%              Clarify there's also no support for < OSX 10.8 anymore.
% 10/04/15 mk  Compatibility fixes for Octave-4, cosmetic cleanups.
% 10/28/15 mk  32-Bit Octave-4 support for MS-Windows reestablished.
% 04/01/16 mk  64-Bit Octave-4 support for MS-Windows established.
% 06/01/16 mk  32-Bit Octave-4 support for MS-Windows removed.
% 11/16/16 mk  Don't bail if a specific 'flavor' is requested, otherwise people
%              can not actually download older PTB versions with this downloader.
% 06/11/18 mk  Change search order for svn executable to account for preferred location
%              on macOS, as provided by XCode command line tools.
% 06/01/19 mk  Give automated hint about updated svn client under Matlab.
% 10/27/20 mk  Add SVN support via Matlabs SVNKit.

% Flush all MEX files: This is needed at least on M$-Windows for SVN to
% work if Screen et al. are still loaded.
clear mex

if nargin < 4 || isempty(downloadmethod)
    downloadmethod = 0;
end

if nargin < 5 || isempty(tryNonInteractiveSetup)
    tryNonInteractiveSetup = 0;
end

oldpause = pause('query');
if tryNonInteractiveSetup
    pause('off');
end

if (nargin < 2) || isempty(flavor) || isempty(strfind(flavor, 'Psychtoolbox-3.0.'))
    % Check if this is 32-Bit Octave-4 on Windows, which we don't support at all:
    if isempty(strfind(computer, 'x86_64')) && ~isempty(strfind(computer, 'mingw32'))
        fprintf('Psychtoolbox 3.0.13 and later do no longer work with 32-Bit GNU/Octave-4 on MS-Windows.\n');
        fprintf('You need to use 64-Bit Octave-5 if you want to use Psychtoolbox with Octave on Windows.\n');
        fprintf('DownloadPsychtoolbox() with flavor ''Psychtoolbox-3.0.12'', does support 32-Bit Octave-4 on Windows.\n');
        error('Tried to setup on 32-Bit Octave, which is no longer supported on Windows.');
    end

    % Check if this is 32-Bit Matlab on Windows or Linux, which we don't support anymore:
    if (strcmp(computer, 'PCWIN') || strcmp(computer, 'GLNX86'))
        fprintf('Psychtoolbox 3.0.12 and later do no longer work with 32-Bit versions of Matlab.\n');
        fprintf('You need to upgrade to a supported 64-Bit version of Octave or Matlab. 32-Bit Octave is still\n');
        fprintf('supported on GNU/Linux.\n');
        fprintf('If you must use a legacy 32-Bit Matlab environment, you can call this function\n');
        fprintf('DownloadPsychtoolbox() with flavor ''Psychtoolbox-3.0.11'', which does support 32-Bit Matlab on Linux and Windows.\n');
        error('Tried to setup on 32-Bit Matlab, which is no longer supported.');
    end

    % Check if this is 32-Bit Octave on OSX, which we don't support anymore:
    if ~isempty(strfind(computer, 'apple-darwin')) && isempty(strfind(computer, '64'))
        fprintf('Psychtoolbox 3.0.11 and later do no longer work with 32-Bit versions of Octave or Matlab on OSX.\n');
        fprintf('You need to upgrade to a 64-Bit version of Octave or Matlab on OSX, which is fully supported.\n');
        fprintf('You can also use the alternate download function DownloadLegacyPsychtoolbox() to download\n');
        fprintf('an old legacy copy of Psychtoolbox-3.0.9, which did support 32-Bit Octave 3.2 on OSX, or use\n');
        fprintf('DownloadPsychtoolbox() with flavor ''Psychtoolbox-3.0.10'', which does support 32-Bit Matlab on OSX.\n');
        error('Tried to setup on 32-Bit Octave, which is no longer supported on OSX.');
    end

    % Check if this is Octave-3 on Windows, which we don't support at all:
    if strcmp(computer, 'i686-pc-mingw32')
        fprintf('Psychtoolbox 3.0.10 and later does no longer work with GNU/Octave-3 on MS-Windows.\n');
        fprintf('You need to use 32-Bit Octave-4 if you want to use Psychtoolbox with Octave on Windows.\n');
        fprintf('Alternatively you can download old - totally unsupported - releases of Psychtoolbox version 3.0.9.\n');
        fprintf('See the ''Versions'' section of our website on how to do that.\n\n');
        error('Tried to setup on Octave-3, which is no longer supported on MS-Windows.');
    end

    if strcmp(computer,'MAC')
        fprintf('This version of Psychtoolbox is no longer supported under MacOSX on the Apple PowerPC hardware platform.\n');
        fprintf('You can get modern versions of Psychtoolbox-3 for Linux if you choose to install GNU/Linux on your PowerPC\n');
        fprintf('machine. These are available from the GNU/Debian project and a future Ubuntu 12.10 release\n.');
        fprintf('Alternatively you can download old - totally unsupported - releases of Psychtoolbox version 3.0.9.\n');
        fprintf('See the ''Versions'' section of our website on how to do that.\n\n');
        error('Apple MacOSX on Apple PowerPC computers is no longer supported by this Psychtoolbox version.');
    end
end

% Check OS
IsWin = ~isempty(strfind(computer, 'PCWIN')) || ~isempty(strfind(computer, '-w64-mingw32'));
IsOSX = ~isempty(strfind(computer, 'MAC')) || ~isempty(strfind(computer, 'apple-darwin'));
IsLinux = strcmp(computer,'GLNX86') || strcmp(computer,'GLNXA64') || ~isempty(strfind(computer, 'linux-gnu'));
IsOctave = isempty (ver('matlab'));

if ~IsWin && ~IsOSX && ~IsLinux
    fprintf('Sorry, this updater doesn''t support your operating system: %s.\n', computer);
    fprintf([mfilename ' can only install Psychtoolbox-3.\n']);
    error(['Your operating system is not supported by ' mfilename '.']);
end

% Check if this is a Matlab of version prior to V 7.4 aka R2007a. The
% absence of verLessThan(), which was introduced in R2007a, signals that.
if ~IsOctave && ~exist('verLessThan') %#ok<EXIST>
    % Matlab version < 7.4 detected. This is no longer supported by current
    % PTB beta. Redirect to the last functional PTB for such ancient Matlab's:
    fprintf('\n\n\n\n');
    fprintf('Psychtoolbox 3.0.10 and later are no longer available for your version of Matlab.\n');
    fprintf('Current versions only work on Matlab Version 7.4 (R2007a) or later.\n');
    fprintf('Please consider upgrading to a recent Matlab version or switching to GNU/Octave 3.2.x.\n');
    fprintf('Both will provide better support, performance and a richer feature set.\n\n');
    fprintf('\n\n');
    fprintf('If you insist on use of an older Matlab version, see our websites ''Versions'' section\n');
    fprintf('on how to retrieve a legacy version of Psychtoolbox 3.0.9 or earlier.\n');
    fprintf('In this case you are entirely on your own, as such versions are not supported in any way.\n\n');
    error('This Downloader does not support Matlab versions before V7.4 (R2007a) anymore.');
end

if nargin < 1
    targetdirectory = [];
end

if isempty(targetdirectory)
    if IsOSX
        % Set default path for OSX install:
        targetdirectory = fullfile(filesep,'Applications');
    else
        % We do not have a default path on Windows, so the user must provide it:
        fprintf('You did not provide the full path to the directory where Psychtoolbox should be\n');
        fprintf('installed. This is required for Microsoft Windows and Linux installation. Please enter a full\n');
        fprintf('path as the first argument to this script, e.g. DownloadPsychtoolbox(''C:\\Toolboxes\\'').\n');
        error('For Windows and Linux, the call to %s must specify a full path for the location of installation.', mfilename);
    end
end

% Strip trailing fileseperator, if any:
if targetdirectory(end) == filesep
    targetdirectory = targetdirectory(1:end-1);
end

if nargin < 3 || isempty(targetRevision)
    targetRevision = '';
else
    fprintf('Target revision: %s \n', targetRevision);
end

% Set flavor default and synonyms:
if nargin < 2 || isempty(flavor)
    flavor = 'beta';
end

% Make sure that flavor is lower-case, unless its a 'Psychtoolbox-x.y.z'
% spec string which is longer than 10 characters and mixed case:
if length(flavor) < 10
    % One of the short flavor spec strings: lowercase'em:
    flavor = lower(flavor);
end

switch (flavor)
    % 'current' is a synonym for 'beta'.
    case 'beta'
    case 'current'
        flavor = 'beta';
    case 'stable'
        fprintf('\n\n\nYou request download of the "stable" flavor of Psychtoolbox.\n');
        fprintf('The "stable" flavor is no longer available.\n');
        error('Flavor "stable" requested. This is no longer available.');
    case 'unsupported'
        fprintf('\n\n\n\n');
        fprintf('Psychtoolbox 3.0.10 and later do no longer provide the "unsupported" flavor.\n');
        error('This Downloader does not support the "unsupported" aka "stable" flavor anymore.');

    otherwise
        fprintf('\n\n\nHmm, requested flavor is the unusual flavor: %s\n',flavor);
        fprintf('Either you request something exotic, or you made a typo?\n');
        fprintf('We will see. If you get an error, this might be the first thing to check.\n');
        fprintf('Press any key to continue...\n');
        pause;
end

fprintf('DownloadPsychtoolbox(''%s'',''%s'',''%s'', %i, %i)\n', targetdirectory, flavor, targetRevision, downloadmethod, tryNonInteractiveSetup);
fprintf('Requested flavor is: %s\n', flavor);
fprintf('Requested location for the Psychtoolbox folder is inside: %s\n', targetdirectory);
switch (downloadmethod)
    case -1
        fprintf('Using svn command line client for subversion checkout.\n');
    case 0
        fprintf('Using optimal client for subversion checkout.\n');
    case 1
        fprintf('Using optimal client for attempt of pure unversioned download.\n');
    otherwise
        error('Unknown downloadmethod specified.\n');
end
fprintf('\n');

% Does SAVEPATH work?
err = savepath;
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
    
    fprintf(['Once "savepath" works (no error message), run ' mfilename ' again.\n']);
    fprintf('Alternatively you can choose to continue with installation, but then you will have\n');
    fprintf('to resolve this permission isssue later and add the path to the Psychtoolbox manually.\n\n');
    if ~tryNonInteractiveSetup
        answer=input('Do you want to continue the installation despite the failure of SAVEPATH (yes or no)? ','s');
    else
        answer='no';
    end

    if ~strcmpi(answer,'yes') && ~strcmpi(answer,'y')
        fprintf('\n\n');
        error('SAVEPATH failed. Please get an administrator to allow everyone to write pathdef.m.');
    end
end

% Do we have sufficient privileges to install at the requested location?
p='Psychtoolbox123test';
[success,m,mm]=mkdir(targetdirectory,p);
if success
    rmdir(fullfile(targetdirectory,p));
else
    fprintf('Write permission test in folder %s failed.\n', targetdirectory);
    if strcmp(m,'Permission denied')
        if IsOSX
            fprintf([
            'Sorry. You would need administrator privileges to install the \n'...
            'Psychtoolbox into the ''%s'' folder. You can either quit now \n'...
            '(say "no", below) and get a user with administrator privileges to run \n'...
            'DownloadPsychtoolbox for you, or you can install now into the \n'...
            '/Users/Shared/ folder, which doesn''t require special privileges. We \n'...
            'recommend installing into the /Applications/ folder, because it''s the \n'...
            'normal place to store programs. \n\n'],targetdirectory);
            if tryNonInteractiveSetup
                answer='yes';
            else
                answer=input('Even so, do you want to install the Psychtoolbox into the \n/Users/Shared/ folder (yes or no)? ','s');
            end

            if ~strcmpi(answer,'yes') && ~strcmpi(answer,'y')
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
        if isempty(p)
            w=what('Psychtoolbox');
            p=w(1).path;
        end
    end
    fprintf('%s\n',p);
    fprintf('That old Psychtoolbox should be removed before we install a new one.\n');
    if ~exist(fullfile(p,'Contents.m'), 'file')
        fprintf(['WARNING: Your old Psychtoolbox folder lacks a Contents.m file. \n'...
            'Maybe it contains stuff you want to keep. Here''s a DIR:\n']);
        dir(p)
    end

    fprintf('First we remove all references to "Psychtoolbox" from the MATLAB / OCTAVE path.\n');
    pp=genpath(p);
    warning('off','MATLAB:rmpath:DirNotFound');
    rmpath(pp);
    warning('on','MATLAB:rmpath:DirNotFound');
    savepath;
    fprintf('Success.\n');

    if tryNonInteractiveSetup
        answer='yes';
    else
        answer=input('Shall I delete the old Psychtoolbox folder and all its contents \n(recommended in most cases), (yes or no)? ','s');
    end

    if strcmpi(answer,'yes') || strcmpi(answer,'y')
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

% Handle Windows ambiguity of \ symbol being the filesep'arator and a
% parameter marker:
if IsWin
    searchpattern = [filesep filesep 'Psychtoolbox[' filesep pathsep ']'];
    searchpattern2 = [filesep filesep 'Psychtoolbox'];
else
    searchpattern  = [filesep 'Psychtoolbox[' filesep pathsep ']'];
    searchpattern2 = [filesep 'Psychtoolbox'];
end

% Remove "Psychtoolbox" from path
while any(regexp(path,searchpattern))
    fprintf('Your old Psychtoolbox appears in the MATLAB / OCTAVE path:\n');
    paths=regexp(path,['[^' pathsep ']*' pathsep],'match');
    fprintf('Your old Psychtoolbox appears %d times in the MATLAB / OCTAVE path.\n',length(paths));
    if tryNonInteractiveSetup
        answer='no';
    else
        answer=input('Before you decide to delete the paths, do you want to see them (yes or no)? ','s');
    end

    if ~strcmpi(answer,'yes') && ~strcmpi(answer,'y')
        fprintf('You didn''t say "yes", so I''m taking it as no.\n');
    else
        for p = paths
            s = char(p);
            if any(regexp(s, searchpattern2))
                fprintf('%s\n',s);
            end
        end
    end

    if tryNonInteractiveSetup
        answer='yes';
    else
        answer = input('Shall I delete all those instances from the MATLAB / OCTAVE path (yes or no)? ', 's');
    end

    if ~strcmpi(answer, 'yes') && ~strcmpi(answer, 'y')
        fprintf('You didn''t say yes, so I cannot proceed.\n');
        fprintf('Please use the MATLAB "File:Set Path" command to remove all instances of "Psychtoolbox" from the path.\n');
        error('Please remove Psychtoolbox from MATLAB / OCTAVE path.');
    end
    for p = paths
        s = char(p);
        if any(regexp(s, searchpattern2))
            % fprintf('rmpath(''%s'')\n',s);
            if strcmpi(s, pwd)
                % Octave 6.4+: rmpath can not remove current working directory from path, so switch it:
                cd('..');
            end
            rmpath(s);
        end
    end
%    savepath;

    fprintf('Success.\n\n');
end

% Download Psychtoolbox
if IsOSX
    fprintf('I will now download the latest Psychtoolbox for OSX.\n');
else
    if IsLinux
        fprintf('I will now download the latest Psychtoolbox for Linux.\n');
    else
        fprintf('I will now download the latest Psychtoolbox for Windows.\n');
    end
end

fprintf('Requested flavor is: %s\n', flavor);
fprintf('Target folder for installation: %s\n', targetdirectory);
p = fullfile(targetdirectory, 'Psychtoolbox');

if ~strcmp(flavor, 'trunk')
    dflavor = ['branches/' flavor];
else
    dflavor = flavor;
end

% Download via Subversion:
svndownload(targetRevision, dflavor, p, downloadmethod);


% Add Psychtoolbox to MATLAB / OCTAVE path
fprintf('Now adding the new Psychtoolbox folder (and all its subfolders) to your MATLAB / OCTAVE path.\n');
p = fullfile(targetdirectory, 'Psychtoolbox');
pp = genpath(p);
addpath(pp);

err = savepath;
if err
    fprintf('SAVEPATH failed. Psychtoolbox is now already installed and configured for use on your Computer,\n');
    fprintf('but i could not save the updated MATLAB / OCTAVE path, probably due to insufficient permissions.\n');
    fprintf('You will either need to fix this manually via use of the path-browser (Menu: File -> Set Path),\n');
    fprintf('or by manual invocation of the savepath command (See help savepath). The third option is, of course,\n');
    fprintf('to add the path to the Psychtoolbox folder and all of its subfolders whenever you restart MATLAB / OCTAVE.\n\n\n');
else 
    fprintf('Success.\n\n');
end

fprintf(['Now setting permissions to allow everyone to write to the Psychtoolbox folder. This will \n'...
        'allow future updates by every user on this machine without requiring administrator privileges.\n']);
try
    % Recursively add write privileges for all users.
    if IsOSX || IsLinux
        [s, m] = fileattrib(p, '+w', 'a', 's');
    else
        [s, m] = fileattrib(p, '+w', '', 's');
    end
catch
    s = 0;
    m = 'Setting file attributes is not supported under Octave.';
end

if s
    fprintf('Success.\n\n');
else
    fprintf('\nFILEATTRIB failed. Psychtoolbox will still work properly for you and other users, but only you\n');
    fprintf('or the system administrator will be able to run the UpdatePsychtoolbox script to update Psychtoolbox,\n');
    fprintf('unless you or the system administrator manually set proper write permissions on the Psychtoolbox folder.\n');
    fprintf('The error message of FILEATTRIB was: %s\n\n', m);
end

fprintf('You can now use your newly installed ''%s''-flavor Psychtoolbox. Enjoy!\n', flavor);
fprintf('Whenever you want to upgrade your Psychtoolbox to the latest ''%s'' version, just\n', flavor);
fprintf('run the UpdatePsychtoolbox script.\n\n');

if exist('PsychtoolboxPostInstallRoutine.m', 'file')
   % Notify the post-install routine of the download and its flavor.
   clear PsychtoolboxPostInstallRoutine;
   PsychtoolboxPostInstallRoutine(0, flavor);
end

pause(oldpause);

% Puuh, we are done :)
return

function svndownload(targetRevision, dflavor, p, downloadmethod)
    % Matlab R2014b (Version 8.4) or later? No legacy download via
    % downloadmethod == -1 requested by user?
    if (downloadmethod ~= -1) && ~isempty(ver('matlab')) && ~verLessThan('matlab', '8.4')
        % R2014b and later contain a Java SVN implementation, so lets use
        % that to spare the user from having to install a separate svn
        % command line client:

        % Get svn_client object from Matlab's Java implementation:
        svn_client_manager = org.tmatesoft.svn.core.wc.SVNClientManager.newInstance;
        svn_client = svn_client_manager.getUpdateClient;

        % Build download URL:
        url = org.tmatesoft.svn.core.SVNURL.parseURIDecoded(['https://github.com/Psychtoolbox-3/Psychtoolbox-3/' dflavor '/Psychtoolbox/']);

        % Build revision to checkout:
        if isempty(targetRevision)
            targetRevision = 'HEAD';
        end
        revision = org.tmatesoft.svn.core.wc.SVNRevision.parse(targetRevision);
        if revision == org.tmatesoft.svn.core.wc.SVNRevision.UNDEFINED
            error('Invalid ''targetRevision'' parameter ''%s'' specified. Not a valid revision spec!', targetRevision);
        end

        %dbglogger = svn_client.getDebugLog();
        %dbgstream = dbglogger.createLogStream(org.tmatesoft.svn.util.SVNLogType.DEFAULT, );

        %try
            if downloadmethod == 0
                % Do the full SVN working copy checkout:
                fprintf('Downloading via Matlabs integrated SVNKit: This can take multiple minutes.\nThere may be no output to this window to indicate progress until the download is complete.\nPlease be patient ...\n');
                revactual = svn_client.doCheckout(url, java.io.File(p), org.tmatesoft.svn.core.wc.SVNRevision.UNDEFINED, revision, org.tmatesoft.svn.core.SVNDepth.INFINITY, true);
            else
                % Do an export without SVN versioning info, ie. essentially a dumb download:
                fprintf('Exporting unversioned copy via Matlabs integrated SVNKit: This can take multiple minutes.\nThere may be no output to this window to indicate progress until the download is complete.\nPlease be patient ...\n');
                revactual = svn_client.doExport(url, java.io.File(p), org.tmatesoft.svn.core.wc.SVNRevision.UNDEFINED, revision, 'LF', true, org.tmatesoft.svn.core.SVNDepth.INFINITY);
            end

            fprintf('Download of SVN revision %i succeeded!\n\n', revactual);
        %catch
        %    disp(psychlasterror('reset'));
        %    error('Download failed! See error messages above.\n\n');
        %end

        return;
    end

    % Fallback path for Octave or older Matlab versions - svn command line client:

    % Check for alternative install location of Subversion:
    if ispc
        % Search for Windows executable in path:
        svnpath = which('svn.exe');
    else
        % Search for Unix executable in path:
        svnpath = which('svn.');
    end

    % Found one?
    if ~isempty(svnpath)
        % Extract basepath and use it:
        svnpath = [fileparts(svnpath) filesep];
    else
        % Could not find svn executable in path. Check the default install
        % locations on OSX and abort if it isn't there. On others we simply
        % have to hope that it is in some system dependent search path.
        if ismac
            svnpath = '';

            % /usr/local/bin is our best shot, because that's where
            % HomeBrew will install its svn client:
            if isempty(svnpath) && exist('/usr/local/bin/svn', 'file')
                svnpath = '/usr/local/bin/';
            end

            % XCode would install its svn in /usr/bin/, except the iToys
            % company f$#ked it up as of the Catalina trainwreck:
            if isempty(svnpath) && exist('/usr/bin/svn', 'file')
                % Spot check if svn works:
                rc = system('/usr/bin/svn');
                if rc == 72
                    % Failed in the expected way under the assumption of a
                    % broken svn executable from XCode:
                    fprintf('The Subversion command line client "svn" on your system is broken. Thanks Apple!\n\n');

                    % Is HomeBrew installed and functional?
                    rc = system('brew --version');
                    if rc == 0
                        % Yes. Try to install Subversion from HomeBrew:
                        fprintf('\nYou do seem to have HomeBrew installed. I can use it now to install a Subversion client.\n');
                        answer = input('Do you want me to install Subversion from HomeBrew now [ie., brew install subversion] (yes or no)? ','s');
                        if strcmpi(answer, 'yes') || strcmpi(answer, 'y')
                            rc = system('brew install subversion');
                            if rc == 0 && exist('/usr/local/bin/svn', 'file')
                                svnpath = '/usr/local/bin/';
                                fprintf('Success! Will use the svn client from /usr/local/bin, ie. from HomeBrew.\n');
                            else
                                fprintf('Failed! I give up fixing this myself, good luck!\n');
                                svnpath = '';
                            end
                        else
                            fprintf('You did not say yes, so i take it as a no. I give up fixing this myself, good luck!\n');
                            svnpath = '';
                        end
                    else
                        % No. Clue the user into installing it to get svn
                        % working:
                        fprintf('You may be able to install HomeBrew (https://brew.sh)\n');
                        fprintf('and then install Subversion via ''brew install subversion''.\n');
                        svnpath = '';
                    end
                else
                    svnpath = '/usr/bin/';
                end
            end

            % From here on unlikely fallback locations:
            if isempty(svnpath) && exist('/bin/svn', 'file')
                svnpath = '/bin/';
            end

            if isempty(svnpath) && exist('/opt/local/bin/svn', 'file')
                svnpath = '/opt/local/bin/';
            end

            if isempty(svnpath) && exist('/opt/subversion/bin/svn', 'file')
                svnpath = '/opt/subversion/bin/';
            end

            if isempty(svnpath)
                fprintf('The Subversion command line client "svn" is not in its expected\n');
                fprintf('location on your disk. On old macOS versions, please download and install\n');
                fprintf('the most recent Subversion client via typing this into a terminal window:\n');
                fprintf('xcode-select --install\n');
                fprintf('and then run %s again. This will not work on Catalina and later though.\n', mfilename);
                fprintf('On Catalina and later you may be able to install HomeBrew (https://brew.sh)\n');
                fprintf('and then install Subversion via ''brew install subversion''.\n');
                error('Subversion client is missing. Please install it.');
            end
        end
    end

    if ~isempty(svnpath)
        fprintf('Will use the svn client which is located in this folder: %s\n', svnpath);
    end

    if any(isspace(svnpath))
        fprintf('WARNING! There are spaces (blanks) in the path to the svn client executable (see above).\n');
        fprintf('On some systems this can cause a download failure, with some error message that may look\n');
        fprintf('roughly like this: %s is not recognized as an internal or external command,\n', svnpath(1:find(isspace(svnpath), 1)));
        fprintf('operable program or batch file.\n\n');
        fprintf('Should the download fail with such a message then move/install the svn.exe program into a\n');
        fprintf('folder whose path does not contain any blanks/spaces and retry.\n\n');
        warning('Spaces in path to subversion client -- May cause download failure.');
    end

    if ~isempty(targetRevision)
        targetRevision = [' -r ' targetRevision ' '];
    end

    % Create quoted version of 'p'ath, so blanks in path are handled properly:
    pt = strcat('"', p, '"');

    checkoutcommand = [svnpath 'svn checkout ' targetRevision ' https://github.com/Psychtoolbox-3/Psychtoolbox-3/' dflavor '/Psychtoolbox/ ' pt];

    fprintf('The following CHECKOUT command asks the Subversion client to \ndownload the Psychtoolbox:\n');
    fprintf('%s\n', checkoutcommand);
    fprintf('Downloading. This can take multiple minutes.\nAlas there may be no output to this window to indicate progress until the download is complete.\nPlease be patient ...\n');
    fprintf('If you see some message asking something like "accept certificate (p)ermanently, (t)emporarily? etc."\n');
    fprintf('then please press the p key on your keyboard, possibly followed by pressing the ENTER key.\n\n');

    if isempty(ver('matlab'))
        % Octave's system() command (and its dos() and unix() wrappers around system())
        % does not print any live output from the checkoutcommand if return of the 'result'
        % string is requested. We want some live feedback, so users get some feeling of
        % download progress and don't get confused if the thing is just sitting there for
        % minutes without giving feedback. Therefore don't request 'result':
        err = system(checkoutcommand);
        result = 'For reasons and troubleshooting, read the output above and all followup messages!';
    else
        % Matlab's system() command can provide live feedback from 'checkoutcommand'
        % during svn checkout and return the same output in 'result' at the end, so
        % we can get 'result' for parsing:
        [err, result] = system(checkoutcommand, '-echo');
    end

    if err
        fprintf('Sorry, the download command "CHECKOUT" failed with error code %d:\n', err);
        fprintf('%s\n', result);

        if ismac && err == 69
            fprintf('If the error output suggests running a command, this should be typed into Terminal.app found in Applications/Utilities\n')
        end

        if isempty(ver('matlab'))
            fprintf('If the error output above contains the text ''SSL handshake failed: SSL error: tlsv1 alert protocol version''\n');
            fprintf('then your svn command line client is too old. Install a more recent Subversion command line client.\n');
        else
            if ~isempty(strfind(result, 'tlsv1 alert protocol version'))
                fprintf('Seems your svn command line client is too old. Install a more recent Subversion command line client.\n');
                error('Download failed. Subversion client too old.');
            end
        end
        fprintf('The download failure might also be due to temporary network or server problems. You may want to try again in a\n');
        fprintf('few minutes. It could also be that the subversion client was not (properly) installed. On Microsoft\n');
        fprintf('Windows you will need to exit and restart Matlab or Octave after installation of the Subversion client. If that\n');
        fprintf('does not help, you will need to reboot your machine before proceeding.\n');
        fprintf('Another reason for download failure could be if an old working copy - a Psychtoolbox folder - still exists.\n');
        fprintf('In that case, it may help to manually delete that folder. Or maybe you do not have write permissions for the target folder?\n\n');
        error('Download failed.');
    end
    fprintf('Download succeeded!\n\n');
return
