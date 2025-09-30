function PsychtoolboxPostInstallRoutine(isUpdate, flavor)
% PsychtoolboxPostInstallRoutine(isUpdate [, flavor])
%
% Psychtoolbox post installation routine. You should not call this
% function directly! This routine is called by DownloadPsychtoolbox,
% or UpdatePsychtoolbox after a successfull download/update of
% Psychtoolbox, or by SetupPsychtoolbox after an in-place installtion.
% The routine performs tasks that are common to downloads and updates,
% so they can share their code/implementation.
%
% As PsychtoolboxPostInstallRoutine itself is downloaded or updated,
% it can contain code specific to each Psychtoolbox revision/release
% to perform special setup procedures for new features, to announce
% important info to the user, whatever...
%
% Currently the routine performs the following tasks:
%
% 1. Clean up the Matlab/Octave path to Psychtoolbox: Remove unneeded .svn subfolders.
% 2. Add the PsychJava subfolder to the static Matlab class-path if neccessary.
%    This enables the Java-based GetChar support on Matlab.
% 3. Add the PsychStartup.m routine to Matlab's and Octave's startup.m file on Windows.
% 4. Set the proper pathes to Psychtoolbox and its mex files.
% 5. Perform post-installation checks, configuration and basic troubleshooting.
% 6. Perform first time software license activation management for user accounts
%    on operating system and machine setups for which this is needed to use
%    the software.

%
% History:
% 23/06/2006 Written (MK).
% 17/09/2006 Made working on Matlab-5 and Octave. Made more robust. (MK)
% 22/09/2006 Replace system copy commands by Matlabs copyfile() - More
%            robust (MK).
% 14/10/2006 Update web page pointers at end, just to point at new wiki (DHB).
% 28/10/2006 Accept 'current' as synonym for 'beta'. (DHB)
% 23/05/2007 Add Matlab R2007 vs. earlier detection to Windows version (MK).
% 16/04/2008 Write/Read PTB flavor to/from users PsychtoolboxConfigDir as well for higher robustness (MK).
% 15/04/2009 Add warning about unsupported OS/X systems older than Tiger (MK).
% 15/06/2009 Add support for postinstall for Octave-3.2.x, remove Octave-2 support (MK).
% 03/01/2010 Extend Matlab R2007 vs. earlier detection for Windows up to year 2014 (MK).
% 03/04/2010 Move PsychtoolboxRegistration to the end, after all real
%            installation has been finished. Add additional error-checking
%            and troubleshooting for peculiarities of Octave on Windows. (MK)
% 05/27/2010 Update instructions for downloading the vcredist_x86.exe
%            security update for users of MS-Windows. (MK)
% 12/27/2010 Add check for unsupported Matlab versions prior to V6.5. (MK)
% 10/31/2011 Add call to SwitchToNewPsychtoolboxHoster for switch to
%            GoogleCode, if needed. (MK)
% 01/06/2012 Add support for calling PsychLinuxConfiguration on Linux. (MK)
% 04/30/2012 Add support for 64-Bit OSX. (MK)
% 06/13/2012 Removed call to SwitchToNewPsychtoolboxHoster, no longer needed (DN)
% 07/10/2012 Use textscan() on R2012a+ and verLessThan() to detect R2007a+ (MK)
% 09/05/2012 Update support for 64-Bit Octave and versions > 3.2. (MK)
% 09/13/2012 Add startup.m setup for 64-Bit Matlab + 64-Bit Windows. (MK)
% 09/14/2012 Cancel support for Octave on MS-Windows. (MK)
% 09/14/2012 Cancel support for 32-Bit Octave on OSX. (MK)
% 11/11/2012 More cleanup. E.g., don't warn about Octave > 3.2 anymore. (MK)
% 04/16/2013 Use javaclasspath.txt instead of classpath.txt on R2013a and later. (MK)
% 05/13/2013 Factor out Java classpath setup to call to PsychJaveTrouble(1). (MK)
% 07/02/2013 Drop support for 32-Bit Matlab on OSX, and thereby for 32-Bit OSX. (MK)
% 07/02/2013 Drop support for OSX versions older than 10.6 "Snow Leopard". (MK)
% 09/12/2013 Setup PsychStartup.m in startup.m for 32-Bit Windows as well. (MK)
% 05/18/2014 No support for 32-Bit Matlab on Linux and Windows anymore for 3.0.12. (MK)
% 09/23/2014 No support for OSX 10.7 and earlier anymore. (MK)
% 10/05/2014 Add some request for donations at the end. (MK)
% 10/17/2015 Also add call to PsychStartup() to Octave startup for MS-Windows. (MK)
% 01/27/2016 Use Octave3 folder for mex files for both Octave-3 and Octave-4. (MK)
% 03/15/2016 Need liboctave-dev package for symlinks liboctinterp.so -> Octave specific liboctinterp.x.so (MK)
% 04/01/2016 64-Bit Octave-4 support for MS-Windows established. (MK)
% 05/13/2016 Add new rpath fixup for Octave on OSX - Copy runtime libs to search dirs. (MK)
% 06/01/2016 32-Bit Octave-4 support for MS-Windows removed. (MK)
%            64-Bit Octave-3 support for OSX removed. (MK)
%            OSX 10.8 and 10.9 support removed. (MK)
% 07/06/2016 No upstream support for 32-Bit Octave on Linux anymore. NeuroDebian provides it though. (MK)
% 12/26/2016 No support for OSX 10.10 anymore. No support for Octave-4.0 on non-Linux,
%            support 64-Bit Octave 4.2 on Windows and OSX instead. (MK)
% 01/03/2017 Fix Matlab incompatibility with __octave_config_info__. (MK)
% 04/07/2018 Remove PsychtoolboxRegistration for now. (MK)
% 07/27/2019 64-Bit Octave 5.1.0 support for Windows and OSX, no Octave-4 support anymore. (MK)
% 10/29/2020 64-Bit Octave 5.2.0 support for Windows and OSX. (MK)
% ??/??/2021 64-Bit Octave 6.1.0 support for Windows and OSX. (MK)
% 02/05/2023 64-Bit Octave 7.3.0 support for Windows and OSX. (MK)
% 03/12/2023 64-Bit Octave 8.1.0 support for OSX. (MK)
% 12/16/2023 Guard against running on macOS with Matlab/Octave for native Apple Silicon. (MK)
% 11/24/2024 Call the 1st time setup routine PsychLicenseHandling() as
%            needed to enable license management on Apple macOS and
%            Microsoft Windows. (MK)
% 01/31/2025 Remove subfolder MatlabWindowsFilesR2007a for Matlab on Windows, and associated
%            special path setup on Matlab for Windows. (MK)
% 02/28/2025 Call PsychLicenseHandling() for license manager client libs setup before 1st
%            call to WaitSecs, so those get installed early enough if they are missing. (MK)
% 05/24/2025 Disable call to PsychPaidSupportAndServices(). Update Linux requirements wrt.
%            Ubuntu 22.04-LTS minimum. (MK)
% 09/25/2025 Update for PTB 3.0.22.2 and Octave 10+ on macOS. (MK)

fprintf('\n\nRunning post-install routine...\n\n');

if nargin < 1
   error('PsychtoolboxPostInstallRoutine: Required argument isUpdate missing!');
end

if nargin < 2
    % No flavor provided: Default to 'unknown', but try to determine it from the
    % flavor file if this is an update.
    flavor = 'unknown';
    try
        if isUpdate>0
            % This is an update of an existing working copy. Check if flavor-file
            % is available:
            flavorfile = [PsychtoolboxRoot 'ptbflavorinfo.txt'];
            if exist(flavorfile, 'file')
                fd=fopen(flavorfile);
                if fd > -1
                    flavor = fscanf(fd, '%s');
                    fclose(fd);
                end
            end

            % Still unknown?
            if strcmp(flavor, 'unknown')
                % Yep: Retry in users PsychtoolboxConfigDir:
                flavorfile = [PsychtoolboxConfigDir 'ptbflavorinfo.txt'];
                if exist(flavorfile, 'file')
                    fd=fopen(flavorfile);
                    if fd > -1
                        flavor = fscanf(fd, '%s');
                        fclose(fd);
                    end
                end
            end
        end
    catch %#ok<*CTCH>
        fprintf('Info: Failed to determine flavor of this Psychtoolbox. Not a big deal...\n');
    end
else
    % Handle 'current' as synonym for 'beta', and 'unsupported' as synonym
    % for former 'stable'.
    switch (flavor)
        case 'current'
            flavor = 'beta';
        case 'unsupported'
            flavor = 'stable';
    end

    % Flavor provided: Write it into the flavor file for use by later update calls:
    try
        flavorfile = [PsychtoolboxRoot 'ptbflavorinfo.txt'];
        fd=fopen(flavorfile, 'wt');
        if fd > -1
            fprintf(fd, '%s\n', flavor);
            fclose(fd);
        end
    catch
        fprintf('Info: Failed to store flavor of this Psychtoolbox to file. Not a big deal...\n');
        % Retry with users PsychtoolboxConfigDir:
        try
            flavorfile = [PsychtoolboxConfigDir 'ptbflavorinfo.txt'];
            fd=fopen(flavorfile, 'wt');
            if fd > -1
                fprintf(fd, '%s\n', flavor);
                fclose(fd);
            end
        catch
            fprintf('Info: Failed to store flavor of this Psychtoolbox to file a 2nd time. Not a big deal...\n');
        end
    end
end

% Get rid of any remaining .svn folders in the path.
try
    path(RemoveSVNPaths);
    savepath;
catch
    fprintf('Info: Failed to remove .svn subfolders from path. Not a big deal...\n');
end

% 32-Bit Octave or 32-Bit Matlab on OSX? This is unsupported as of Version 3.0.11.
if ~IsLinux && ~Is64Bit
    fprintf('Psychtoolbox no longer works with 32-Bit versions of Octave or Matlab on macOS or Windows.\n');
    fprintf('You need to upgrade to a 64-Bit version of Octave or Matlab on these systems, which is fully supported.\n');
    fprintf('Psychtoolbox-3.0.9 was the last to support 32-Bit Octave 3.2 on macOS.\n');
    fprintf('Psychtoolbox-3.0.10 was the last to support 32-Bit Matlab on macOS.\n');
    fprintf('Psychtoolbox-3.0.12 was the last to support 32-Bit Octave-4 on Windows.\n');
    error('Tried to setup on 32-Bit Octave or Matlab, which is no longer supported on macOS or Windows.');
end

if IsLinux && ~Is64Bit && IsOctave && ~IsARM
    fprintf('Psychtoolbox no longer works with 32-Bit Octave for Intel on Linux.\n');
    fprintf('The only exception is 32-Bit Octave for Linux on ARM processors like the RaspberryPi.\n');
    fprintf('Not to worry though, you can get a supported Psychtoolbox 3.0.15+ for 32-Bit Octave\n');
    fprintf('on Linux from the NeuroDebian project if you run Debian GNU/Linux or a Ubuntu flavor.\n');
    fprintf('Go to this link for installation instructions:\n');
    fprintf('http://neuro.debian.net/pkgs/octave-psychtoolbox-3.html#pkg-octave-psychtoolbox-3\n\n');
    fprintf('\n\nPress any key to continue after you have read and understood above message.\n\n');
    pause;
end

if ~Is64Bit && ~IsOctave
    fprintf('Psychtoolbox no longer works with 32-Bit versions of Matlab.\n');
    fprintf('You need to upgrade to a supported 64-Bit version of Octave or Matlab. 32-Bit Octave is still\n');
    fprintf('supported on GNU/Linux.\n');
    fprintf('If you must use a legacy 32-Bit Matlab environment, then legacy\n');
    fprintf('Psychtoolbox-3.0.11 does support 32-Bit Matlab on Linux and Windows.\n');
    error('Tried to setup on 32-Bit Matlab, which is no longer supported.');
end

% Check if our own startup function is part of the startup file and add it,
% if it isn't already part of it. Currently we only need this for MS-Windows.
if IsWin
    % Is it already implanted? Then we ain't nothing to do:
    if ~IsPsychStartupImplantedInStartup
        % Nope. Does a proper file already exist?
        if IsOctave
            whereisit = [get_home_directory filesep '.octaverc'];
            if exist(whereisit, 'file') == 0
                whereisit = '';
            end
        else
          whereisit = which('startup.m');
        end

        if isempty(whereisit)
            % No: Create our own one.
            if IsOctave
                whereisit = [get_home_directory filesep '.octaverc'];
                fprintf('Creating a .octaverc file for Psychtoolbox at %s\n', whereisit);
            else
                whereisit = [PsychtoolboxRoot 'PsychInitialize' filesep 'startup.m'];
                fprintf('Creating a startup.m file for Psychtoolbox at %s\n', whereisit);
            end
        else
            fprintf('Adding PsychStartup() call to startup file for Psychtoolbox at %s\n', whereisit);
        end

        % whereisit points to the location of the existing or to be created
        % file. Open (or create) it in append mode:
        try
            fd = fopen(whereisit, 'a');
            fprintf(fd, '\n');
            fprintf(fd, '%% Call Psychtoolbox-3 specific startup function:\n');
            fprintf(fd, 'if exist(''PsychStartup''), PsychStartup; end;\n');
            fprintf(fd, '\n');
            fclose(fd);
        catch
            fprintf('WARNING: Failed to update or create startup file to add a call to PsychStartup()! Trouble ahead.\n');
        end
    end

    % Execute our startup function once manually, so it works already for
    % this session:
    PsychStartup;
end

% Check for operating system minor version on Mac OS/X.
if IsOSX
    % Query kernel version via system() call:
    [s, did]=system('uname -r');
    if s == 0
        % Parse string for kernel major number, then translate to OS
        % minor version by subtracting 4:
        minorver = sscanf(did, '%i') - 4;
    else
        % Failed to query: Assume we're good for now...
        minorver = inf;
    end

    % Is the operating system version < 10.13?
    if minorver < 13
        % Yes. This is macOS 10.12 or earlier, i.e., older than 10.13
        % High Sierra. PTB will not work on such an old system:
        fprintf('\n\n\n\n\n\n\n\n==== WARNING WARNING WARNING WARNING ====\n\n');
        fprintf('Your operating system is Mac OS/X version 10.%i.\n\n', minorver);
        fprintf('This release of Psychtoolbox-3 is not compatible\n');
        fprintf('to macOS versions older than 10.13 "High Sierra".\n');
        fprintf('That means that almost all functionality will not work!\n\n');
        fprintf('You could download an older version of Psychtoolbox\n');
        fprintf('onto your system to get better results. See our Wiki for help.\n');
        fprintf('Better though, update your operating system to at least version\n');
        fprintf('10.13, better macOS 13 or later.\n');
        fprintf('\n\n\n==== WARNING WARNING WARNING WARNING ====\n\n\n');
        fprintf('Press any key on keyboard to try to continue with setup, although\n');
        fprintf('this will likely fail soon and leave you with a dysfunctional toolbox.\n\n');
        pause;
    end

    % Make sure we have a suitable fontconfig config file so fontconfig can
    % find the fonts on macOS, even if libfontconfig from GStreamer is
    % used, instead of from HomeBrew or X11:
    if ~exist([PsychHomeDir '.fonts.conf'], 'file')
        [rc, msg] = copyfile([PsychtoolboxRoot 'PsychBasic/macOS_fontconfig.fonts.conf'], [PsychHomeDir '.fonts.conf']);
        if rc
            fprintf('FontConfig configuration file for DrawText %s was missing. Successfully installed a suitable file.\n', [PsychHomeDir '.fonts.conf']);
        else
            fprintf('ERROR: FontConfig configuration file for DrawText %s was missing, but failed to install a suitable file!\n', [PsychHomeDir '.fonts.conf']);
            fprintf('ERROR: This may cause the Screen(''DrawText'') function to fail, because it may not find fonts.\n');
            fprintf('ERROR: Type ''help DrawTextPlugin'' for potential troubleshooting help.\n');
            fprintf('ERROR: The error message of the failed copyfile command was the following:\n''%s''\n', msg);
        end
    end
end

% Check if this is Matlab of a version prior to V7.4: verLessThan was introduced in v7.4 / R2007a:
if ~IsOctave && ~exist('verLessThan') %#ok<EXIST>
    % Matlab version < 7.4 detected. This is no longer supported.
    fprintf('\n\nYou are using a Matlab version older than Version 7.4.\n');
    fprintf('The current Psychtoolbox is no longer compatible with your version of Matlab.\n');
    fprintf('It only works on Matlab Version 7.4 (R2007a) or later.\n\n');
    fprintf('Please run the legacy DownloadLegacyPsychtoolbox() downloader to download an outdated,\n');
    fprintf('but functional older version of Psychtoolbox (e.g., V3.0.9) for your Matlab setup or to\n');
    fprintf('receive further instructions.\n');
    fprintf('Matlab versions older than v7.4 aka R2007a are no longer supported. Setup aborted.\n');
    return;
end

% LexActivator client library installed? Install if needed.
PsychLicenseHandling('CheckInstallLMLibs');

% Special case handling for Octave:
if IsOctave
    % GNU/Octave. Need to prepend the proper folder with
    % the pseudo-MEX files to path:
    rc = 0;
    rdir = '';

    try
        % Remove binary MEX folders from path:
        if exist([PsychtoolboxRoot 'PsychBasic' filesep 'Octave3LinuxFilesARM'], 'dir')
            rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave3LinuxFilesARM']);
        end

        if exist([PsychtoolboxRoot 'PsychBasic' filesep 'Octave5LinuxFiles64'], 'dir')
            rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave5LinuxFiles64']);
        end

        if exist([PsychtoolboxRoot 'PsychBasic' filesep 'Octave5LinuxFiles64' filesep 'Wayland'], 'dir')
            rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave5LinuxFiles64' filesep 'Wayland']);
        end

        if exist([PsychtoolboxRoot 'PsychBasic' filesep 'Octave8OSXFiles64'], 'dir')
            rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave8OSXFiles64']);
        end

        if exist([PsychtoolboxRoot 'PsychBasic' filesep 'Octave8OSXFilesARM64'], 'dir')
            rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave8OSXFilesARM64']);
        end

        if exist([PsychtoolboxRoot 'PsychBasic' filesep 'Octave8WindowsFiles64'], 'dir')
            rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave8WindowsFiles64']);
        end

        if exist([PsychtoolboxRoot 'PsychBasic' filesep 'Octave10OSXFiles64'], 'dir')
            rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave10OSXFiles64']);
        end

        if exist([PsychtoolboxRoot 'PsychBasic' filesep 'Octave10OSXFilesARM64'], 'dir')
            rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave10OSXFilesARM64']);
        end

        % Encode prefix and Octave major version of proper folder:
        octavev = sscanf(version, '%i.%i.%i');
        octavemajorv = octavev(1);
        octaveminorv = octavev(2);
        octavepatchv = octavev(3);

        fprintf('Octave major version %i detected. Will prepend the following folder to your Octave path:\n', octavemajorv);

        % Octave on Linux with ARM processor, e.g., RaspberryPi?
        if IsLinux && IsARM
            % 32-Bit ARM can currently share mex files from Octave 3.8 to at least 6.2,
            % so treat it as Octave 3.8, and all versions will share the same folder:
            octavemajorv = 3;
            octaveminorv = 8;
        end

        if ((octavemajorv >= 5) || (octavemajorv == 4 && octaveminorv >= 4)) && IsLinux
            % Octave-4.4, 5.x, 6.x, 7.x, 8.x and Octave-9.x can share the same mex files in the Octave-5 folder on Linux:
            rdir = [PsychtoolboxRoot 'PsychBasic' filesep 'Octave5'];
        elseif ismember(octavemajorv, [3,4]) && IsLinux
            % Octave-3 and Octave-4.0/4.2 can share the same mex files in the Octave-3 folder on Linux:
            rdir = [PsychtoolboxRoot 'PsychBasic' filesep 'Octave3'];
            if ~IsARM
                % Only warn on non-ARM ie. not RaspberryPi:
                fprintf('\nOctave versions < 4.4 are no longer supported on Linux. This will likely fail!\n');
                fprintf('Upgrade to Ubuntu 22.04-LTS or an equivalent modern Linux distribution.\n');
                fprintf('Press any key to confirm you read and understand this message.\n');
                pause;
            end
        elseif (octavemajorv >= 10) && IsOSX
            % Octave 10 and later can share the same mex files built against Octave 10:
            rdir = [PsychtoolboxRoot 'PsychBasic' filesep 'Octave10'];
        else
            % Everything else (aka other OS'es) goes by Octave major version:
            %rdir = [PsychtoolboxRoot 'PsychBasic' filesep 'Octave' num2str(octavemajorv)];
            % Override - Always use the Octave8WindowsFiles64 folder, although for actual Octave7 mex files:
            rdir = [PsychtoolboxRoot 'PsychBasic' filesep 'Octave8'];
        end

        % Add proper OS dependent postfix:
        if IsLinux
            rdir = [rdir 'LinuxFiles'];
        end

        if IsOSX
            rdir = [rdir 'OSXFiles'];
        end

        if IsWin
            rdir = [rdir 'WindowsFiles'];
        end

        if IsARM
            % ARM processor architecture:
            rdir = [rdir 'ARM'];
        end

        if Is64Bit
            % 64 bit Octave. Select 64 bit mex file folder:
            rdir = [rdir '64'];
        end

        fprintf(' %s ...\n', rdir);
        addpath(rdir);

        % No Wayland specific subfolder on path by default:
        if exist([rdir filesep 'Wayland'], 'dir')
            rmpath([rdir filesep 'Wayland']);
        end

        rc = savepath;
    catch
        rc = 2;
    end

    if rc > 0
        fprintf('=====================================================================\n');
        fprintf('ERROR: Failed to prepend folder %s to Octave path!\n', rdir);
        fprintf('ERROR: This will likely cause complete failure of PTB to work.\n');
        fprintf('ERROR: Please fix the problem (maybe insufficient permissions?)\n');
        fprintf('ERROR: If everything else fails, add this folder manually to the\n');
        fprintf('ERROR: top of your Octave path.\n');
        fprintf('ERROR: Trying to continue but will likely fail soon.\n');
        fprintf('=====================================================================\n\n');
    end

    if  (IsOSX && (octavemajorv < 10)) || ...
        (IsWin && (octavemajorv ~= 7 || ~ismember(octaveminorv, 3))) || ...
        (IsLinux && ((octavemajorv < 4 && ~IsARM) || (octavemajorv == 4 && octaveminorv < 4) || (octavemajorv > 8)))
        fprintf('\n\n===============================================================================================\n');
        fprintf('WARNING: Your version %s of Octave is incompatible with this release. We strongly recommend\n', version);
        if IsLinux
            % On Linux everything from 4.4 to at least 9.x is fine:
            fprintf('WARNING: using the latest stable version of the Octave 4.4, 5.x, 6.x, 7.x, 8.x or 9.x series.\n');
            fprintf('WARNING: You can get Psychtoolbox for other, or more recent, versions of Octave from NeuroDebian.\n');
        elseif IsOSX
            fprintf('WARNING: only using Octave 10 and likely later versions with this version of Psychtoolbox.\n');
        else
            % On Windows we only care about 7.3 atm:
            fprintf('WARNING: only using Octave 7.3 with this version of Psychtoolbox.\n');
        end
        fprintf('WARNING: Stuff may not work at all or only suboptimal with other versions and we\n');
        fprintf('WARNING: don''t provide any support for such old versions.\n');
        fprintf('\nPress any key to continue with setup.\n');
        fprintf('===============================================================================================\n\n');
        pause;
    end

    if IsOSX
        % Need to symlink the Octave mex runtime library somewhere our mex files can find it. The only low-maintenance
        % way of dealing with this mess of custom library pathes per octave version, revision and packaging format.
        % Preferred location is the folder with our mex files - found by the @rpath = @loader_path encoded in our mex files.
        tdir = PsychHomeDir('lib');
        dummy = unlink([tdir 'liboctinterp.dylib']); %#ok<*NASGU>
        dummy = unlink([tdir 'liboctave.dylib']);
        dummy = unlink([tdir 'liboctmex.dylib']);
        dummy = unlink([rdir filesep 'liboctinterp.dylib']);
        dummy = unlink([rdir filesep 'liboctave.dylib']);
        dummy = unlink([rdir filesep 'liboctmex.dylib']);
        if symlink([GetOctlibDir filesep 'liboctmex.dylib'], [rdir filesep 'liboctmex.dylib'])
            % Symlink from our mex files folder failed. A second location where the linker will search is the
            % $HOME/lib directory of the current user, so try that as target location:
            fprintf('\n\nFailed to symlink Octave mex runtime library to mex file folder [%s].\nRetrying in users private lib dir: %s ...\n', rdir, tdir);
            if symlink([GetOctlibDir filesep 'liboctinterp.dylib'], [tdir 'liboctinterp.dylib'])
                fprintf('\nFailed to symlink mex runtime lib to [%s] as well :(.\n', tdir);
                fprintf('Our mex files will likely not work this way. Maybe the directories lack file write permissions?\n');
                fprintf('\n\n\nA last workaround would be to restart octave from a terminal via this line:\n\nexport DYLD_LIBRARY_PATH=%s ; octave\n\n\n', GetOctlibDir);
            end
        end
    end

    try
        % Rehash the Octave toolbox cache:
        path(path);
        rehash;
        clear WaitSecs;
    catch
        fprintf('WARNING: Rehashing the Octave toolbox cache failed. I may fail and recommend\n');
        fprintf('WARNING: quitting and restarting Octave, then retry.\n');
    end

    try
        % Try if WaitSecs MEX file works...
        WaitSecs(0.1);
    catch
        % Failed! Either screwed setup of path or missing runtime
        % libraries.
        fprintf('ERROR: WaitSecs-MEX does not work, most likely other MEX files will not work either.\n');
        if ismember(octavemajorv, [3,4]) && IsLinux
            fprintf('ERROR: Make sure to have the ''liboctave-dev'' package installed, otherwise symlinks\n');
            fprintf('ERROR: from liboctinterp.so to the liboctinterp library of your Octave installation\n');
            fprintf('ERROR: might by missing, causing our mex files to fail to load with linker errors.\n');
        end
        fprintf('ERROR: One reason might be that your version %s of Octave is incompatible.\n', version);
        fprintf('ERROR: Another conceivable reason would be missing or incompatible required system libraries on your system.\n\n');
        fprintf('ERROR: After fixing the problem, restart this installation/update routine.\n\n');
        fprintf('\n\nInstallation aborted. Fix the reported problem and retry.\n\n');
        return;
    end

    % End of special Octave setup.
end

% Special case handling for different Matlab releases on MS-Windoze:
if IsWin && ~IsOctave
    if strfind(cd, 'system32') %#ok<STRIFCND>
        % the below code fails if the current directory is system32 (e.g.
        % C:\Windows\system32), as it contains dlls like version.dll, which
        % get called instead of the built-in functions....
        cd(PsychtoolboxRoot);
    end

    % Is this a R2014b (Version 8.4.0) or later Matlab?
    if verLessThan('matlab', '8.4.0')
        % This is a pre-R2014b Matlab: No longer supported by V 3.0.18+
        fprintf('Matlab release prior to R2014b detected. This version is no longer\n');
        fprintf('supported by Psychtoolbox 3.0.18 and later. Aborted.');
        fprintf('\n\nInstallation aborted. Fix the reported problem and retry.\n\n');
        return;
    end

    try
        % Rehash the Matlab toolbox cache:
        path(path);
        rehash('pathreset');
        rehash('toolboxreset');
        clear WaitSecs;
    catch
        fprintf('WARNING: Rehashing the Matlab toolbox cache failed. I may fail and recommend\n');
        fprintf('WARNING: quitting and restarting Matlab, then retry.\n');
    end

    try
        % Try if WaitSecs MEX file works...
        WaitSecs('YieldSecs', 0.1);
    catch
        % Failed! Either screwed setup of path or missing VC++ 2019 runtime libraries.
        fprintf('ERROR: WaitSecs-MEX does not work, most likely other MEX files will not work either.\n');
        fprintf('ERROR: Most likely cause: The Microsoft Visual C++ 2019 runtime libraries\n');
        fprintf('ERROR: are missing on your system.\n\n');
        fprintf('ERROR: Execute the installer file vcredist_x64_2015-2019.exe, which is located in your Psychtoolbox/PsychContributed/ folder.\n');
        fprintf('ERROR: You must execute that installer as an administrator user. Exit Matlab before the installation, then restart it.\n');
        fprintf('ERROR: After fixing the problem, restart this installation/update routine.\n\n');
        fprintf('ERROR: You can also just do a: cd(PsychtoolboxRoot); SetupPsychtoolbox;\n\n');
        fprintf('ERROR: This will avoid a full download of Psychtoolbox over the internet and just finish the setup.\n');

        fprintf('\n\nInstallation aborted. Fix the reported problem and retry.\n\n');
        return;
    end
end

% If we're using Matlab then add the PsychJava stuff to the static
% Java classpath.
if ~IsOctave
    % Try to setup Matlab static Java class path:
    PsychJavaTrouble(1);
end

try
    % Time for one-time license setup for this user account on this operating system
    % and machine combination, if this is required for the given Psychtoolbox
    % variant. Otherwise it is a no-op:
    if ~PsychLicenseHandling('Setup')
        % A required software licensing setup procedure failed, or there isn't
        % any valid and active license for this machine. The mex files will not
        % work, so there is no point in continuing with setup:
        fprintf('Psychtoolbox setup aborted due to failed software licensing setup or machine not licensed.\n');
        return;
    end
catch
    warning('Software licensing setup failed for some reason. This may end badly soon...');
end

% Check if Screen is functional:
try
    % Linux specific instructions:
    if IsLinux
        more(pause('query'));
        fprintf('\n\n');
        fprintf('The Psychtoolbox on GNU/Linux needs the following 3rd party libraries\n');
        fprintf('in order to function correctly. If you get "Invalid MEX file errors",\n');
        fprintf('or similar fatal error messages, check if these are installed on your\n');
        fprintf('system and if they are missing, install them via your system specific\n');
        fprintf('software management tools.\n');
        fprintf('On a Debian/Ubuntu based system, you may get the system to install all these\n');
        fprintf('required dependencies for you by issuing the following commmand in a terminal:\n\n');
        fprintf('sudo apt build-dep psychtoolbox-3\n');
        fprintf('\n');
        fprintf('For Screen() and OpenGL support:\n\n');
        fprintf('* The OpenGL utility toolkit GLUT: glut, glut-3 or freeglut are typical provider packages in most Linux distributions.\n');
        fprintf('\n');
        fprintf('* GStreamer multimedia framework: At least version 1.20 of the core runtime and the gstreamer-base plugins.\n');
        fprintf('  For optimal performance and the full set of features, please use the latest available versions.\n');
        fprintf('  You may need to install additional packages to playback all common audio and video file formats.\n');
        fprintf('  See "help GStreamer" for more info.\n');
        fprintf('\n');
        fprintf('* libusb-1.0 USB low-level access library.\n');
        fprintf('\n');
        fprintf('* libdc1394 IEEE-1394 Firewire and USB-Vision IIDC video capture library, version libdc1394.25.or later.\n');
        fprintf('\n');
        fprintf('* libraw1394 Firewire low-level access library.\n');
        fprintf('\n\n');
        fprintf('For PsychKinect() (See "help InstallKinect"):\n\n');
        fprintf('* libusb-1.0 USB low-level access library.\n\n');
        fprintf('* libfreenect-0.5: Kinect driver library version 0.5 or later.\n');
        fprintf('\n\n');
        fprintf('For PsychHID() support:\n\n');
        fprintf('* libusb-1.0 USB low-level access library.\n');
        fprintf('\n\n');
        fprintf('For PsychPortAudio() support:\n\n');
        fprintf('* libportaudio.so.2 Portaudio sound library.\n');
        fprintf('\n\n');
        fprintf('For Eyelink():\n\n');
        fprintf('* The Eyelink core libraries from the SR-Research download website.\n');
        fprintf('\n');
        fprintf('\n');
        fprintf('If you receive an installation failure soon, then please read the output of\n');
        fprintf('"help GStreamer" first and follow the installation instructions for GStreamer\n');
        fprintf('on Linux. Psychtoolbox''s Screen() command will not work without GStreamer!\n\n');
        more off;
    end

    % Check Screen:
    AssertOpenGL;

    if IsLinux && ~IsARM
        % A blast from the past - Unlikely to be needed nowadays. Definitely skip on RaspberryPi.
        % Setup Desktop compositor ("Compiz") to un-redirect fullscreen windows.
        % This allows to bypass desktop composition for PTB fullscreen onscreen windows,
        % so we get the deterministic timing and high performance we want for visual
        % stimulus presentation. The command is a no-op
        PsychGPUControl('FullScreenWindowDisablesCompositor', 1);
    end

    if ~IsLinux
        % If this is not Linux, then open an invisible onscreen window and
        % draw some dummy text with the plugin text renderer, just to
        % trigger a (re-)build of the fontconfig cache if that should prove
        % neccessary. Such a rebuild can take many seconds to even minutes
        % and looks to the user like a hang, so do it here, with proper
        % warning. We skip this on Linux, as the OS will take care of
        % proper rebuilds appropriately, so we don't expect to ever run
        % into this problem:
        fprintf('\n\nTrying to trigger an update of the fontconfig cache if that should prove neccessary.\n');
        fprintf('This may take a couple of seconds, or sometimes even minutes. Please be patient...\n');
        drawnow;

        oldRenderer = Screen('Preference', 'TextRenderer', 1);
        oldLevel = Screen('Preference', 'WindowShieldingLevel', -1);
        oldVerbo = Screen('Preference', 'Verbosity', 0);
        oldTsMode = Screen('Preference', 'VBLTimestampingmode', -1);
        oldSkip = Screen('Preference', 'SkipSyncTests', 2);
        % Need to suppress OpenGL error checking during run, as
        % 'WindowShieldingLevel' -1 for hiding the Window triggers new bugs
        % in the OS of the iToys company, this time if running on macOS
        % 10.14 or earlier! Error is non-consequential so suppress:
        oldCVS = Screen('Preference', 'ConserveVRAM', 512);

        try
            win = Screen('OpenWindow', 0, 0, [0 0 100 100]);
            Screen('DrawText', win, 'Ola!');
            Screen('Flip', win);
            sca;

            if Screen('Preference', 'TextRenderer') == 0
                % Failed to load drawtext plugin.
                warning('Something went wrong with high-quality text renderer setup. Read ''help DrawTextPlugin'' for troubleshooting.');
                disp('Press Return key to continue.');
                pause;
            end
        catch
            warning('Something went wrong with high-quality text renderer setup. Read ''help DrawTextPlugin'' for troubleshooting.');
            disp('Press Return key to continue.');
            pause;
        end

        Screen('Preference', 'TextRenderer', oldRenderer);
        Screen('Preference', 'WindowShieldingLevel', oldLevel);
        Screen('Preference', 'Verbosity', oldVerbo);
        Screen('Preference', 'VBLTimestampingmode', oldTsMode);
        Screen('Preference', 'SkipSyncTests', oldSkip);
        Screen('Preference', 'ConserveVRAM', oldCVS);
    end

    % Tell user we're successfully done:
    fprintf('\nDone with post-installation. Psychtoolbox is ready for use.\n\n\n');
catch
    fprintf('\n\n');
    fprintf('Screen() failed to work for some reason:\n\n');
    if IsWin
      fprintf('On Windows you *must* install the MSVC build runtime of at least GStreamer 1.20.5\n');
      fprintf('or a later version. Screen() will not work with earlier versions, or without GStreamer,\n');
      fprintf('or with the MinGW variants of the GStreamer runtime!\n');
      fprintf('Read ''help GStreamer'' for more info.\n\n');
    end
    fprintf('Check the troubleshooting instructions on our Wiki (Download section \n');
    fprintf('and FAQ section, maybe also the Bugs section).\n\n');
    fprintf('Once you manage to fix the problem (simply type ''AssertOpenGL'' to verify\n');
    fprintf('that stuff works now), you do not need to run the installation routine again,\n');
    fprintf('but can start working immediately.\n\n');
    fprintf('Press RETURN or ENTER to confirm you read and understood the above message.\n');
    pause;
    fprintf('\n\n');
end

% Run Linux post-configuration script try-catch protected:
if IsLinux
    try
        % This script modifies/extends system configuration files
        % to optimize the system for use with Psychtoolbox:
        PsychLinuxConfiguration([], 1);
    catch
    end
end

% Some goodbye, copyright and getting started blurb...
more(pause('query'));
fprintf('GENERAL LICENSING CONDITIONS AND TERMS OF USE:\n');
fprintf('----------------------------------------------\n\n');
fprintf('Almost all of the material contained in the Psychtoolbox-3 distribution\n');
fprintf('is free-software and/or open-source software under a OSI (http://www.opensource.org/)\n');
fprintf('approved license. Most material is covered by the MIT license or a MIT compatible license.\n\n');

fprintf('A few internal libraries and components are covered by other free software\n');
fprintf('licenses which we understand to be compatible with the MIT license, e.g., the GNU LGPL\n');
fprintf('license, or BSD and Apache-2.0 licenses, or they are in the public domain.\n\n');
fprintf('3rd-party components which are freely useable and redistributable for non-commercial\n');
fprintf('research use, due to the authors permissions, but are not neccessarily free / open-source\n')
fprintf('software, can be found in the "PsychContributed" subfolder of the Psychtoolbox distribution,\n');
fprintf('accompanied by their respective licenses.\n\n');

fprintf('A few components are licensed under the GNU GPL v2 license with a special linking\n');
fprintf('exception for use with Mathworks proprietary Matlab application. A very few components,\n');
fprintf('could be covered in non-standard build configurations by the standard GPL v2 license and\n');
fprintf('cannot be used with Matlab. These are only available for use with GNU/Octave.\n\n');

fprintf('Please read the license text and copyright info in the Psychtoolbox file\n');
fprintf('License.txt carefully before you use or redistribute Psychtoolbox-3.\n');
fprintf('Use of Psychtoolbox-3 components implies that you have read, understood and accepted\n');
fprintf('the licensing conditions.\n\n');

fprintf('Psychtoolbox and its prebuilt mex files are distributed in the hope that they will be useful, but\n');
fprintf('WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n');

fprintf('If you have any questions with respect to data protection and the privacy policy related to use of\n');
fprintf('the prebuilt mex files bundled with Psychtoolbox, as far as network licensing is used on your\n');
fprintf('operating system and configuration, please type "help PsychLicenseHandling".\n\n');
fprintf('However, in a nutshell, if you just use Psychtoolbox for your research, our licenses\n');
fprintf('don''t restrict you in any practically relevant way. Commercial users, developers or\n');
fprintf('redistributors should make sure they understand the licenses for the components they use.\n');
fprintf('If in doubt, contact one of the Psychtoolbox developers, or the original authors of the\n');
fprintf('components you want to use, modify, merge or redistribute with other software.\n\n');
fprintf('Your standard Psychtoolbox distribution comes without the source code for\n');
fprintf('the binary plugins (the MEX files). If you want to access the corresponding\n');
fprintf('source code, please type "help UseTheSource" for specific download instructions.\n\n');
fprintf('BEGINNERS READ THIS:\n');
fprintf('--------------------\n\n');
fprintf('If you are new to the Psychtoolbox, you might try this: \nhelp Psychtoolbox\n\n');
fprintf('Psychtoolbox website, which also links to a tutorial for basic use cases:\n');
fprintf('web http://www.psychtoolbox.org -browser\n');
fprintf('\n');
fprintf('Please make sure that you have a look at the detailed ECVP 2013 tutorial slides\n');
fprintf('in the PDF file PTBTutorial-ECVP2013.pdf\n');
fprintf('in the Psychtoolbox/PsychDocumentation subfolder for an overview of differences\n');
fprintf('between Psychtoolbox-2 and Psychtoolbox-3 and proper use of basic features. That\n');
fprintf('folder contains various additional helpful information for use of Psychtoolbox.\n\n');
fprintf('\n');
fprintf('Please also familiarize yourself with the demos contained in the PsychDemos subfolder\n');
fprintf('and its subfolders. They show best practices for many common tasks and are generally\n');
fprintf('well documented.\n');

% Our little ad for our services:
if exist('PsychPaidSupportAndServices', 'file')
    % PsychPaidSupportAndServices(1);
end

more off;

fprintf('\nEnjoy!\n\n');
fprintf('Press RETURN or ENTER to confirm you read and understood the above message.\n');
pause;
fprintf('\n\n');

% Clear out everything:
if IsWin
    clear all; %#ok<CLALL>
end

if IsOSX
    try
        % Dummy call to trigger PsychHID on OSX, which in turn may trigger
        % the diagnostic and troubleshooting instructions for Apples latest
        % macOS Catalina+ security bullshit - Windows Vista, Apple edition:
        KbCheck;
    catch
        clear PsychHID;
    end
end

return;
