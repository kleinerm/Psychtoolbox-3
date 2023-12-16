function UpdatePsychtoolbox(targetdirectory, targetRevision, tryNonInteractiveSetup)
% UpdatePsychtoolbox([targetdirectory][, targetRevision][, tryNonInteractiveSetup=0])
%
% Update your working copy of the Psychtoolbox with the latest bug fixes,
% enhancements, and features from our Git server.
%
% If you are using a Psychtoolbox provided by NeuroDebian, then this is not
% needed. You will be automatically notified of updates to Psychtoolbox by
% your operating systems update manager as soon as they become available.
%
% The "targetdirectory" argument is optional. If present, it specifies the path
% of the Psychtoolbox folder to update. If omitted, UpdatePsychtoolbox will
% update the Psychtoolbox folder reported by PsychtoolboxRoot(). Examples:
%
% UpdatePsychtoolbox
% UpdatePsychtoolbox('~/Applications/Psychtoobox')
%
% The "targetRevision" argument is optional and should be normally omitted.
% Normal behaviour is to upgrade your working copy to the latest revision.
% If you provide a specific targetRevision, then this script will
% *downgrade* your copy of Psychtoolbox to the specified revision. This is
% only useful if you experience problems after an update and want to revert
% to an earlier known-to-be-good release. Revisions can be specified by a
% revision number or by the special flag 'PREV' which should downgrade to
% the revision before the most current one. By executing this script
% multiple times with the 'PREV' specifier, you could incrementally
% downgrade until stuff works for you.
%
% UpdatePsychtoolbox cannot change the beta-vs-stable flavor of your
% Psychtoolbox. To change the flavor, run DownloadPsychtoolbox again.
%
% The optional parameter 'tryNonInteractiveSetup' if provided as 1 (true), will
% try a setup without user interaction, not asking users for input in certain
% situations, but assuming an answer that keeps the setup progressing. Note that
% this is not guaranteed to work in all cases, and may end in data loss, e.g.,
% overwriting an old and potentially user-modified Psychtoolbox installation.
% This non-interactice setup mode is highly experimental, not well tested, not
% supported in case of any trouble!
%

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
% 05/27/12 mk  Strip backwards compatibility support to pre-R2007a.
%              Remove PowerPC support.
%
% 09/14/12 mk  Drop support for Octave on MS-Windows.
% 09/14/12 mk  Drop support for 32-Bit Octave on OSX.
% 07/02/13 mk  Drop support for 32-Bit Matlab on OSX, and thereby for 32-Bit OSX.
% 05/18/14 mk  No support for 32-Bit Matlab on Linux and Windows anymore for 3.0.12.
% 10/28/15 mk  32-Bit Octave-4 support for MS-Windows reestablished.
% 04/01/16 mk  64-Bit Octave-4 support for MS-Windows established.
% 06/01/16 mk  32-Bit Octave-4 support for MS-Windows removed.
% 06/01/19 mk  Give automated hint about updated svn client under Matlab.
% 10/31/19 dgp Allows UpdatePsychtoolbox to run without Psychtoolbox in path. 
% 12/18/19 mk  Add "--accept theirs-full" to svn update, so in case of conflicts,
%              server provided upstream files will just override/overwrite user
%              modified files. Not super-friendly of us, but may cut down support
%              overhead.
% 10/28/20 mk  Add SVN support via Matlabs SVNKit.
% 12/16/23 mk  Add warning about, and handling of GitHub SVN frontend shutdown at 8th January 2024.

addpath(fullfile(fileparts(mfilename('fullpath')),'PsychOneliners'));

% Flush all MEX files: This is needed at least on MS-Windows for SVN to
% work if Screen et al. are still loaded.
clear mex

if nargin < 1
    targetdirectory = [];
end

if isempty(targetdirectory)
    targetdirectory = PsychtoolboxRoot;
end

% Strip trailing fileseperator, if any:
if targetdirectory(end) == filesep
    targetdirectory = targetdirectory(1:end-1);
end

if nargin < 2 || isempty(targetRevision)
    targetRevision = '';
else
    fprintf('Target revision: %s \n', targetRevision);
end

if nargin < 3 || isempty(tryNonInteractiveSetup)
    tryNonInteractiveSetup = 0;
end

% It's the end of the world as we know it...
if datenum(date) > datenum('8-Jan-2024') %#ok<*DATE,*DATNM>
    fprintf('This Updater does no longer work after 8th January 2024, as GitHub has removed its\n');
    fprintf('Subversion download frontend from their services permanently at that day.\n\n');
else
    fprintf('This Updater will no longer work, starting at or after 8th January 2024, as GitHub will\n');
    fprintf('remove its Subversion download frontend from their services permanently at that day.\n');
    fprintf('Before this deadline, there may be multiple temporary hour-long or day-long brownouts,\n');
    fprintf('resulting in update failure, as a heads-up by GitHub about the upcoming permanent end of service.\n\n');
end

fprintf('Due to the lack of financial support for Psychtoolbox by the vast majority of our non-paying users,\n');
fprintf('we did not and currently do not have the funding to work on a good alternative solution for this.\n');
fprintf('Therefore this convenient installation and update method will be unavailable for an unknown period\n');
fprintf('of time.\n\n');
fprintf('See http://psychtoolbox.org/download.html#alternate-download for a workable, although way less\n');
fprintf('convenient and advanced download and installation method, via zip file download and execution\n');
fprintf('of SetupPsychtoolbox(). Good luck!\n\n\n');

if datenum(date) > datenum('8-Jan-2024')
    error('Updater aborted due to currently unsupported update method.');
end

oldpause = pause('query');
if tryNonInteractiveSetup
    pause('off');
end

fprintf('UpdatePsychtoolbox(''%s'') \n', targetdirectory);
fprintf('\n');

% Do notify user about potential trouble with path names with blanks in them:
if any(isspace(targetdirectory))
    fprintf('The targetdirectory spec contains white-space. This should work, but has not been tested extensively.\n');
end

% Check if this is 32-Bit Octave-4 on Windows, which we don't support at all:
if isempty(strfind(computer, 'x86_64')) && ~isempty(strfind(computer, 'mingw32'))
    fprintf('Psychtoolbox 3.0.13 and later do no longer work with 32-Bit GNU/Octave-4 on MS-Windows.\n');
    fprintf('You need to use 64-Bit Octave-5.2.0 if you want to use Psychtoolbox with Octave on Windows.\n');
    fprintf('DownloadPsychtoolbox() with flavor ''Psychtoolbox-3.0.12'', does support 32-Bit Octave-4 on Windows.\n');
    error('Tried to setup on 32-Bit Octave, which is no longer supported on Windows.');
end

% Check if this is 32-Bit Matlab on Windows or Linux, which we don't support anymore:
if (strcmp(computer, 'PCWIN') || strcmp(computer, 'GLNX86'))
    fprintf('Psychtoolbox 3.0.12 and later do no longer work with 32-Bit versions of Matlab.\n');
    fprintf('You need to upgrade to a supported 64-Bit version of Octave or Matlab. 32-Bit Octave is still\n');
    fprintf('supported on GNU/Linux by NeuroDebian or Linux distribution repositories.\n');
    fprintf('If you must use a legacy 32-Bit Matlab environment, you can call this function\n');
    fprintf('DownloadPsychtoolbox() with flavor ''Psychtoolbox-3.0.11'', which does support 32-Bit Matlab on Linux and Windows.\n');
    error('Tried to setup on 32-Bit Matlab, which is no longer supported.');
end

% Check if this is 32-Bit Octave or 32-Bit Matlab on OSX, which we don't support anymore:
if (~isempty(strfind(computer, 'apple-darwin')) || strcmp(computer,'MACI')) && isempty(strfind(computer, '64'))
    fprintf('Psychtoolbox 3.0.11 and later do no longer work with 32-Bit versions of Octave or Matlab on OSX.\n');
    fprintf('You need to upgrade to a 64-Bit version of Octave or Matlab on OSX, which is fully supported.\n');
    fprintf('You can also use the alternate download function DownloadLegacyPsychtoolbox() to download\n');
    fprintf('an old legacy copy of Psychtoolbox-3.0.9, which did support 32-Bit Octave 3.2 on OSX, or use\n');
    fprintf('DownloadPsychtoolbox() with flavor ''Psychtoolbox-3.0.10'', which does support 32-Bit Matlab on OSX.\n');
    error('Tried to setup on 32-Bit Octave or 32-Bit Matlab, which are no longer supported on OSX.');
end

% Check if this is Octave-3 on Windows, which we don't support at all:
if strcmp(computer, 'i686-pc-mingw32')
    fprintf('Psychtoolbox 3.0.10 and later do no longer work with GNU/Octave-3 on MS-Windows.\n');
    fprintf('You need to use 64-Bit Octave-5 if you want to use Psychtoolbox with Octave on Windows.\n');
    fprintf('You can also use the alternate download function DownloadLegacyPsychtoolbox() to download\n');
    fprintf('an old legacy copy of Psychtoolbox-3.0.9 which did support 32-Bit Octave 3.2 on Windows.\n');
    error('Tried to setup on Octave, which is no longer supported on MS-Windows.');
end

if strcmp(computer,'MAC')
    fprintf('This version of Psychtoolbox is no longer supported under MacOSX on the Apple PowerPC hardware platform.\n');
    error('Apple MacOSX on Apple PowerPC computers is no longer supported by this Psychtoolbox version.');
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

% Save old Psychtoolbox path:
oldPath = RemoveSVNPaths(genpath(targetdirectory));

% get current path and only remove those folders that are currently on
% path, to prevent some unnecessary warnings:
oldPath = PathListIsMember(oldPath, path);

% Matlab R2014b (Version 8.4) or later?
if ~IsOctave && ~verLessThan('matlab', '8.4')
    % R2014b and later contain a Java SVN implementation, so lets use
    % that to spare the user from having to install a separate svn
    % command line client:

    % Get svn_client object from Matlab's Java implementation:
    svn_client_manager = org.tmatesoft.svn.core.wc.SVNClientManager.newInstance;
    svn_client = svn_client_manager.getUpdateClient;

    % Build revision to checkout:
    if isempty(targetRevision)
        targetRevision = 'HEAD';
    end

    revision = org.tmatesoft.svn.core.wc.SVNRevision.parse(targetRevision);
    if revision == org.tmatesoft.svn.core.wc.SVNRevision.UNDEFINED
        error('Invalid ''targetRevision'' parameter ''%s'' specified. Not a valid revision spec!', targetRevision);
    end

    % Do the SVN working copy update:
    fprintf('Updating via Matlabs integrated SVNKit: This can take multiple minutes.\nThere may be no output to this window to indicate progress until the update is complete.\nPlease be patient ...\n');
    revactual = svn_client.doUpdate(java.io.File(targetdirectory), revision, org.tmatesoft.svn.core.SVNDepth.INFINITY, true, true);
    fprintf('Update to SVN revision %i succeeded!\n\n', revactual);
else
    % Fallback path for Octave or older Matlab versions - svn command line client:
    if ~isempty(targetRevision)
        targetRevision = [' -r ' targetRevision ' '];
    end

    % Retrieve path to Subversion executable:
    svnpath = GetSubversionPath;

    fprintf('About to update your working copy of the OpenGL-based Psychtoolbox-3.\n');
    updatecommand=[svnpath 'svn update --accept theirs-full '  targetRevision ' ' strcat('"',targetdirectory,'"') ];
    fprintf('Will execute the following update command:\n');
    fprintf('%s\n', updatecommand);

    if IsOctave
        % Octave's system() command (and its dos() and unix() wrappers around system())
        % does not print any live output from the checkoutcommand if return of the 'result'
        % string is requested. We want some live feedback, so users get some feeling of
        % download progress and don't get confused if the thing is just sitting there for
        % minutes without giving feedback. Therefore don't request 'result':
        err = system(updatecommand);
        result = 'For reasons and troubleshooting, read the output above and all followup messages!';
    else
        % Matlab's system() command can provide live feedback from 'checkoutcommand'
        % during svn checkout and return the same output in 'result' at the end, so
        % we can get 'result' for parsing:
        [err, result] = system(updatecommand, '-echo');
    end

    if err
        fprintf('Sorry. The update command failed with error code %d:\n', err);
        fprintf('%s\n', result);

        if IsOSX && err == 69
            fprintf('If the error output suggests running a command, this should be typed into Terminal.app found in Applications/Utilities\n')
        end

        if IsOctave
            fprintf('If the error output above contains the text ''SSL handshake failed: SSL error: tlsv1 alert protocol version''\n');
            fprintf('then your svn command line client is too old. Install a more recent Subversion command line client.\n');
        else
            if ~isempty(strfind(result, 'tlsv1 alert protocol version'))
                fprintf('Seems your svn command line client is too old. Install a more recent Subversion command line client.\n');
            end
        end

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
    fprintf('If you cannot resolve such a conflict, the simplest solution is to manually\n');
    fprintf('delete the file or subfolder for which a conflict is reported, and then run\n');
    fprintf('UpdatePsychtoolbox again. It will download and add the proper missing files.\n');
    fprintf('If everything else fails, simply delete the whole Psychtoolbox folder and use\n');
    fprintf('DownloadPsychtoolbox again for a full, clean download.\n');
    fprintf('\n');
end

% Remove old Psychtoolbox paths. Add new Psychtoolbox paths.
rmpath(oldPath);
addpath(genpath(targetdirectory));
fprintf('Your MATLAB/OCTAVE path has been updated. Now trying to save the new MATLAB/OCTAVE path...\n\n');
err = savepath;
if err
    try
        % If this works then we're likely on Matlab:
        p=fullfile(matlabroot,'toolbox','local','pathdef.m');
        fprintf(['Sorry, SAVEPATH failed. Probably the pathdef.m file lacks write permission. \n'...
                 'Please ask a user with administrator privileges to enable \n'...
                 'write by everyone for the file:\n\n''%s''\n\n'],p);
    catch %#ok<CTCH>
        % Probably on Octave:
        fprintf(['Sorry, SAVEPATH failed. Probably your ~/.octaverc file lacks write permission. \n'...
                 'Please ask a user with administrator privileges to enable \n'...
                 'write by everyone for that file.\n\n']);
    end

    fprintf(['Once that''s done, run ' mfilename ' again. For this session, Psychtoolbox\n']);
    fprintf('will be fully functional, but you will need to save your path settings to make them persistent.\n\n');
end

% Does a post-install routine exist? If so, we execute it, now that PTB is basically ready.
if exist('PsychtoolboxPostInstallRoutine.m', 'file')
   % We pass the information about downloaded flavor and that this is a download (=0) to the
   % post-install routine...
   clear PsychtoolboxPostInstallRoutine;
   PsychtoolboxPostInstallRoutine(1);
end

pause(oldpause);

return
