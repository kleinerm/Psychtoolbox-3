function SetupPsychtoolbox(tryNonInteractiveSetup)
% SetupPsychtoolbox - In-place setup of PTB without network access.
%
% SetupPsychtoolbox([tryNonInteractiveSetup=0])
%
% This script prepares an already downloaded working copy of Psychtoolbox
% for use with Matlab or Octave. It sets proper paths and takes care of
% special setup operations for various things.
%
% The optional parameter 'tryNonInteractiveSetup' if provided as 1 (true), will
% try a setup without user interaction, not asking users for input in certain
% situations, but assuming an answer that keeps the setup progressing. Note that
% this is not guaranteed to work in all cases, and may end in data loss, e.g.,
% overwriting an old and potentially user-modified Psychtoolbox installation.
% This non-interactice setup mode is highly experimental, not well tested, not
% supported in case of any trouble!
%
% Installation is a three step procedure:
%
% 1. Download and unpack a full working copy of PTB into your target
% folder. Obviously you need to somehow get a copy, either via conventional
% download from a computer with network connection, visit this URL for that
% download: http://psychtoolbox.org/download.html
% Or from a helpful colleague or copied from some other machine.
%
% 2. Change your Matlab/Octave working directory to the Psychtoolbox installation
% folder, e.g., 'cd /Applications/Psychtoolbox'.
%
% 3. Type 'SetupPsychtoolbox' to run this script.
%
%
% If you get stuck, post your question to the forum:
%
% https://psychtoolbox.discourse.group
%
% Please specify your full name and the version of your operating system,
% MATLAB or Octave and Psychtoolbox.
%

% History:
%
% 01/25/07 mk  Created. Derived from DownloadPsychtoolbox. Basically a
%              stripped down version of it.
%
% 05/31/09 mk  Add support for Octave-3.
% 06/07/09 mk  Apply smallish fix to fix counting, contributed by Chuan Zeng.
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
% 01/25/24 mk  Cleanup, dead code removal.

% Flush all MEX files: This is needed at least on M$-Windows if Screen et al. are still loaded.
clear mex; %#ok<CLMEX>

if nargin < 1 || isempty(tryNonInteractiveSetup)
    tryNonInteractiveSetup = 0;
end

oldpause = pause('query');
if tryNonInteractiveSetup
    pause('off');
end

% Validate supported OS:
IsWin = ispc;
IsOSX = ismac;
IsLinux = isunix && ~ismac;

if ~IsWin && ~IsOSX && ~IsLinux
    fprintf('Sorry, this updater doesn''t support your operating system: %s.\n', computer);
    fprintf([mfilename ' can only install the Linux, Windows and macOS versions of the Psychtoolbox-3.\n']);
    error(['Your operating system is not supported by ' mfilename '.']);
end

% Locate ourselves:
targetdirectory=fileparts(mfilename('fullpath'));
if ~strcmpi(targetdirectory, pwd)
    error('You need to change your working directory to the Psychtoolbox folder before running this routine!');
end

fprintf('Will setup working copy of the Psychtoolbox folder inside: %s\n',targetdirectory);
fprintf('\n');

% Does SAVEPATH work?
err=savepath;
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

% Handle Windows ambiguity of \ symbol being the fileseparator and a parameter marker:
if IsWin
    searchpattern = [filesep filesep 'Psychtoolbox[' filesep pathsep ']'];
    searchpattern2 = [filesep filesep 'Psychtoolbox'];
else
    searchpattern  = [filesep 'Psychtoolbox[' filesep pathsep ']'];
    searchpattern2 = [filesep 'Psychtoolbox'];
end

% Remove "Psychtoolbox" from path:
while any(regexp(path, searchpattern))
    fprintf('Your old Psychtoolbox appears in the MATLAB/OCTAVE path:\n');
    paths=regexp(path,['[^' pathsep ']+'],'match');
    fprintf('Your old Psychtoolbox appears %d times in the MATLAB/OCTAVE path.\n',length(paths));

    if tryNonInteractiveSetup
        answer = 'no';
    else
        answer=input('Before you decide to delete the paths, do you want to see them (yes or no)? ','s');
    end

    if ~strcmpi(answer,'yes') && ~strcmpi(answer,'y')
        fprintf('You didn''t say "yes", so I''m taking it as no.\n');
    else
        for p=paths
            s=char(p);
            if any(regexp(s,searchpattern2))
                fprintf('%s\n',s);
            end
        end
    end

    if tryNonInteractiveSetup
        answer = 'yes';
    else
        answer=input('Shall I delete all those instances from the MATLAB/OCTAVE path (yes or no)? ','s');
    end

    if ~strcmpi(answer,'yes') && ~strcmpi(answer,'y')
        fprintf('You didn''t say yes, so I cannot proceed.\n');
        fprintf('Please use the MATLAB "File:Set Path" command or its Octave equivalent to remove all instances of "Psychtoolbox" from the path.\n');
        error('Please remove Psychtoolbox from MATLAB/OCTAVE path.');
    end
    for p=paths
        s=char(p);
        if any(regexp(s,searchpattern2))
            % fprintf('rmpath(''%s'')\n',s);
            if strcmpi(s, pwd)
                % Octave 6.4+: rmpath can not remove current working directory from path, so switch it:
                cd('..');
            end

            rmpath(s);
        end
    end

    savepath;
    fprintf('Success.\n\n');
end

% Add Psychtoolbox to MATLAB/OCTAVE path
fprintf('Now adding the new Psychtoolbox folder (and all its subfolders) to your MATLAB/OCTAVE path.\n');
p=targetdirectory;
pp=genpath(p);
addpath(pp);

err=savepath;
if err
    fprintf('SAVEPATH failed. Psychtoolbox is now already installed and configured for use on your Computer,\n');
    fprintf('but i could not save the updated Matlab/Octave path, probably due to insufficient permissions.\n');
    fprintf('You will either need to fix this manually via use of the path-browser (Menu: File -> Set Path),\n');
    fprintf('or by manual invocation of the savepath command (See help savepath). The third option is, of course,\n');
    fprintf('to add the path to the Psychtoolbox folder and all of its subfolders whenever you restart Matlab.\n\n\n');
else 
    fprintf('Success.\n\n');
end

if IsOSX
    % Apples trainwreck needs special treatment. If Psychtoolbox has been
    % downloaded via a webbrowser as a zip file or tgz file and then
    % extracted, then all binary executable files like .dylib's and
    % mex files will have the com.apple.quarantine attribute set to
    % prevent them from working in any meaningfully user fixable way -
    % Thanks Apple! Use the xattr command to remove the quarantine flag.
    fprintf('Trying to fixup Apple macOS broken security workflow by removing the quarantine flag from our mex files...\n\n');

    if IsOctave
        % Fix the Octave mex files:
        if IsARM
            [rc, msg] = system(['xattr -d com.apple.quarantine ' p filesep 'PsychBasic/Octave8OSXFilesARM64/*.mex 2>&1']);
        else
            [rc, msg] = system(['xattr -d com.apple.quarantine ' p filesep 'PsychBasic/Octave8OSXFiles64/*.mex 2>&1']);
        end
    else
        % Fix the Matlab mex files:
        [rc, msg] = system(['xattr -d com.apple.quarantine ' p filesep 'PsychBasic/*.' mexext]);
    end

    % Fix the DrawText plugin and other plugins:
    if rc == 0 || ~isempty(strfind(msg, 'xattr: com.apple.quarantine')) %#ok<STREMP>
        if IsOctave
            [rc, msg] = system(['xattr -d com.apple.quarantine ' p filesep 'PsychBasic/PsychPlugins/*.dylib 2>&1']);
        else
            [rc, msg] = system(['xattr -d com.apple.quarantine ' p filesep 'PsychBasic/PsychPlugins/*.dylib']);
        end
    end

    % Worked?
    if rc ~= 0
        if isempty(strfind(msg, 'xattr: com.apple.quarantine')) %#ok<STREMP>
            fprintf('FAILED! Psychtoolbox will likely not work correctly!\n');
            PsychPaidSupportAndServices(2);
            error(['Removing the quarantine flag from our mex files to workaround macOS brokeness failed! Error was: ' msg]);
        end
    end
end

if exist('PsychtoolboxPostInstallRoutine.m', 'file')
   % Notify the post-install routine of the "pseudo-update" It will
   % determine the proper flavor by itself.
   PsychtoolboxPostInstallRoutine(1);
end

pause(oldpause);

% Puuh, we are done :)
return
