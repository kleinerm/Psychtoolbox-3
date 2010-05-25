function PsychtoolboxPostInstallRoutine(isUpdate, flavor)
% PsychtoolboxPostInstallRoutine(isUpdate [, flavor])
%
% Psychtoolbox post installation routine. You should not call this
% function directly! This routine is called by DownloadPsychtoolbox,
% or UpdatePsychtoolbox after a successfull download/update of
% Psychtoolbox. The routine performs tasks that are common to
% downloads and updates, so they can share their code/implementation.
%
% As PsychtoolboxPostInstallRoutine itself is downloaded or updated,
% it can contain code specific to each Psychtoolbox revision/release
% to perform special setup procedures for new features, to announce
% important info to the user, whatever...
%
% Currently the routine performs the following tasks:
%
% 1. Clean up the Matlab path to Psychtoolbox: Remove unneeded .svn subfolders.
% 2. Contact the Psychtoolbox server to perform online registration of this
%    working copy of Psychtoolbox.
% 3. Add the PsychJava subfolder to the static Matlab class-path if neccessary.
%    This enables the Java-based GetChar support on Matlab.

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

fprintf('\n\nRunning post-install routine...\n\n');

if nargin < 1
   error('PsychtoolboxPostInstallRoutine: Required argument isUpdate missing!');
end;

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
    catch
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
    if exist('savepath')
        savepath;
    else
        path2rc;
    end
catch
    fprintf('Info: Failed to remove .svn subfolders from path. Not a big deal...\n');
end

% Check for operating system minor version on Mac OS/X when running under
% Matlab:
if IsOSX
    if ~IsOctave %#ok<AND2>
        % Running on Matlab + OS/X. Find the operating system minor version,
        % i.e., the 'y' in the x.y.z number, e.g., y=3 for 10.3.7:

        % Get 32-digit binary encoded minor version from Gestalt() MEX file:
        binminor = Gestalt('sys2');

        % Decode into decimal digit:
        minorver = 0;
        for i=1:32
            minorver = minorver + binminor(i) * 2^(32-i);
        end
    else
        % Running on Octave + OS/X: Query kernel version via system() call:
        [s, did]=system('uname -r');
        if s == 0
            % Parse string for kernel major number, then translate to OS
            % minor version by subtracting 4:
            minorver = sscanf(did, '%i') - 4;
        else
            % Failed to query: Assume we're good for now...
            minorver = inf;
        end
    end
    
    % Is the operating system minor version 'minorver' < 4?
    if minorver < 4
        % Yes. This is MacOS/X 10.3 or earlier, i.e., older than 10.4
        % Tiger. In all likelihood, this current PTB release won't work on
        % such a system anymore, because some of the binary MEX files are
        % linked against incompatible runtimes and frameworks. Output a
        % clear warning message about this, with tips on how to resolve the
        % problem:
        fprintf('\n\n\n\n\n\n\n\n==== WARNING WARNING WARNING WARNING ====\n\n');
        fprintf('Your operating system is Mac OS/X version 10.%i.\n\n', minorver);
        fprintf('This release of Psychtoolbox-3 is likely no longer compatible\n');
        fprintf('to OS/X versions older than 10.4 "Tiger".\n\n');
        fprintf('That means that some or many crucial functions will fail.\n');
        fprintf('You may encounter errors or failures during the remainder of\n');
        fprintf('this installation procedure, or later on during use of the toolkit.\n\n');
        fprintf('You can fix this problem by switching to the last Psychtoolbox version\n');
        fprintf('that was known to (mostly) work on your old operating system:\n\n');
        fprintf('1. Type clear all on the Matlab command prompt.\n\n');
        fprintf('2. Run the downloader script DownloadPsychtoolbox again,\n');
        fprintf('   (see help DownloadPsychtoolbox for usage information),\n');
        fprintf('   but specify the optional "flavor" argument as:\n\n');
        fprintf('   ''Psychtoolbox-3.0.8-PreTiger''\n\n');
        fprintf('   Example for a standard installation in standard location:\n\n');
        fprintf('   DownloadPsychtoolbox([],[],[],''Psychtoolbox-3.0.8-PreTiger'');\n\n\n');
        fprintf('This will delete your current copy of Psychtoolbox-3 and replace it\n');
        fprintf('by the last known good version for your system.\n\n');
        fprintf('Please note that we will no longer provide support, bug fixes or enhancements\n');
        fprintf('for this old release on your old and obsolete operating system - You are on your own.\n\n');
        fprintf('We strongly recommend that you upgrade your system to a more recent OS/X version soon.\n\n');
        fprintf('Thanks for your attention and good luck!');
        fprintf('\n\n\n==== WARNING WARNING WARNING WARNING ====\n\n\n');
        fprintf('Press any key on keyboard to continue with setup...\n');
        pause;
    end
end

% Special case handling for Octave:
if IsOctave
    % OS/X or Linux under Octave. Need to prepend the proper folder with
    % the pseudo-MEX files to path:
    rc = 0;
    rdir = '';
    
    try
        % Remove binary MEX folders from path:
        rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave3LinuxFiles']);
        rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave3OSXFiles']);
        rmpath([PsychtoolboxRoot 'PsychBasic' filesep 'Octave3WindowsFiles']);
        
        % Encode prefix and Octave major version of proper folder:
        octavev = sscanf(version, '%i.%i');
        octavemajorv = octavev(1);
        octaveminorv = octavev(2);
        
        rdir = [PsychtoolboxRoot 'PsychBasic' filesep 'Octave' num2str(octavemajorv)];
        
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

        fprintf('Octave major version %i detected. Will prepend the following folder to your Octave path:\n', octavemajorv);
        fprintf(' %s ...\n', rdir);
        addpath(rdir);
        
        if exist('savepath')
            rc = savepath;
        else
            rc = path2rc;
        end
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
    
    if octavemajorv < 3 | octaveminorv < 2
        fprintf('\n\n=================================================================================\n');
        fprintf('WARNING: Your version %s of Octave is obsolete. We strongly recommend\n', version);
        fprintf('WARNING: using the latest stable version of at least Octave 3.2.0 for use with Psychtoolbox.\n');
        fprintf('WARNING: Stuff may not work at all or only suboptimal with earlier versions and we\n');
        fprintf('WARNING: don''t provide any support for such old versions.\n');
        fprintf('\nPress any key to continue with setup.\n');
        fprintf('=================================================================================\n\n');
        pause;
    end
    
    try
        % Rehash the Octave toolbox cache:
        path(path);
        rehash;
        clear WaitSecs;
    catch
        fprintf('WARNING: rehashing the Octave toolbox cache failed. I may fail and recommend\n');
        fprintf('WARNING: Quitting and restarting Octave, then retry.\n');
    end
    
    try
        % Try if Screen MEX file works...
        WaitSecs(0.1);
    catch
        % Failed! Either screwed setup of path or missing runtime
        % libraries.
        fprintf('ERROR: WaitSecs-MEX does not work, most likely other MEX files will not work either.\n');
        fprintf('ERROR: One reason might be that your version %s of Octave is incompatible. We recommend\n', version);        
        fprintf('ERROR: use of the latest stable version of Octave-3.2.x as announced on www.octave.org website.\n');
        fprintf('ERROR: Another conceivable reason would be missing or incompatible required system libraries on your system.\n\n');
        fprintf('ERROR: After fixing the problem, restart this installation/update routine.\n\n');
        fprintf('\n\nInstallation aborted. Fix the reported problem and retry.\n\n');
        return;
    end
    
    % End of special Octave setup.
end

% Special case handling for different Matlab releases on MS-Windoze:
if IsWin & ~IsOctave
    rc = 0;
    try
        % Remove DLL folders from path:
        rmpath([PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR11\']);
        rmpath([PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
        
        % Is this a Release2007a or later Matlab?
        if ~isempty(strfind(version, '2007')) | ~isempty(strfind(version, '2008')) | ...
           ~isempty(strfind(version, '2009')) | ~isempty(strfind(version, '2010')) | ...
           ~isempty(strfind(version, '2011')) | ~isempty(strfind(version, '2012')) | ...
           ~isempty(strfind(version, '2013')) | ~isempty(strfind(version, '2014'))
           
            % This is a R2007a or post R2007a Matlab:
            % Add PsychBasic/MatlabWindowsFilesR2007a/ subfolder to Matlab
            % path:
            rdir = [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\'];
            fprintf('Matlab release 2007a or later detected. Will prepend the following\n');
            fprintf('folder to your Matlab path: %s ...\n', rdir);
            addpath(rdir);
        else
            % This is a pre-R2007a Matlab:
            % Add PsychBasic/MatlabWindowsFilesR11/ subfolder to Matlab
            % path:
            rdir = [PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR11\'];
            fprintf('Matlab release prior to R2007a detected. Will prepend the following\n');
            fprintf('folder to your Matlab path: %s ...\n', rdir);
            addpath(rdir);
        end

        if exist('savepath')
            rc = savepath;
        else
            rc = path2rc;
        end
    catch
        rc = 2;
    end

    if rc > 0
        fprintf('=====================================================================\n');
        fprintf('ERROR: Failed to prepend folder %s to Matlab path!\n', rdir);
        fprintf('ERROR: This will likely cause complete failure of PTB to work.\n');
        fprintf('ERROR: Please fix the problem (maybe insufficient permissions?)\n');
        fprintf('ERROR: If everything else fails, add this folder manually to the\n');
        fprintf('ERROR: top of your Matlab path.\n');
        fprintf('ERROR: Trying to continue but will likely fail soon.\n');
        fprintf('=====================================================================\n\n');
    end
    
    try
        % Rehash the Matlab toolbox cache:
        path(path);
        rehash('pathreset');
        rehash('toolboxreset');
        clear WaitSecs;
    catch
        fprintf('WARNING: rehashing the Matlab toolbox cache failed. I may fail and recommend\n');
        fprintf('WARNING: Quitting and restarting Matlab, then retry.\n');
    end
    
    try
        % Try if WaitSecs MEX file works...
        WaitSecs('YieldSecs', 0.1)
    catch
        % Failed! Either screwed setup of path or missing VC++ 2005 runtime
        % libraries.
        fprintf('ERROR: WaitSecs-MEX does not work, most likely other MEX files will not work either.\n');
        fprintf('ERROR: Most likely cause: The Visual C++ 2005 runtime libraries are missing on your system.\n\n');
        fprintf('ERROR: Visit http://www.mathworks.com/support/solutions/data/1-2223MW.html for instructions how to\n');
        fprintf('ERROR: fix this problem. That document tells you how to download and install the required runtime\n');
        fprintf('ERROR: libraries. It is important that you download the libraries for Visual C++ 2005 SP1\n');
        fprintf('ERROR: - The Service Pack 1! Follow the link under the text "For VS 2005 SP1 vcredist_x86.exe:"\n');
        fprintf('ERROR: If you install the wrong runtime, it will still not work.\n\n');
        fprintf('ERROR: After fixing the problem, restart this installation/update routine.\n\n');

        if strcmp(computer,'PCWIN64')
            % 64 bit Matlab running on 64 bit Windows?!? That won't work.
            fprintf('ERROR:\n');
            fprintf('ERROR: It seems that you are running a 64-bit version of Matlab on your system.\n');
            fprintf('ERROR: That won''t work at all! Psychtoolbox currently only supports 32-bit versions\n');
            fprintf('ERROR: of Matlab.\n');
            fprintf('ERROR: You can try to exit Matlab and then restart it in 32-bit emulation mode to\n');
            fprintf('ERROR: make Psychtoolbox work on your 64 bit Windows. You do this by adding the\n');
            fprintf('ERROR: startup option -win32 to the matlab.exe start command, ie.\n');
            fprintf('ERROR: matlab.exe -win32\n');
            fprintf('ERROR: If you do not know how to do this, consult the Matlab help about startup\n');
            fprintf('ERROR: options for Windows.\n\n');
        end
        
        fprintf('\n\nInstallation aborted. Fix the reported problem and retry.\n\n');
        return;
    end
end

% If we're using Matlab then add the PsychJava stuff to the static
% Java classpath.
if ~IsOctave
    try
       % Figure out the PsychJava path we need to add to the static Java
        % classpath.
        path_PsychJava = [PsychtoolboxRoot, 'PsychJava'];

        % Open up the classpath.txt file and find any PsychJava entries.  If
        % they exist, remove them, and put the current one in the file.  This
        % only allows on PsychJava to be on the path.
        classpathFile = which('classpath.txt');
        bakclasspathFile = [classpathFile '.bak'];
        
        fileContents = textread(classpathFile, '%s');
        j = 1;
        newFileContents = {};
        pathInserted = 0;
        for i = 1:length(fileContents)
            % Look for the first instance of PsychJava in the classpath and
            % replace it with the new one.  All other instances will be
            % ignored.
            if isempty(findstr('PsychJava', fileContents{i}))
                newFileContents{j, 1} = fileContents{i};
                j = j + 1;
            elseif ~isempty(findstr('PsychJava', fileContents{i})) & ~pathInserted
                newFileContents{j, 1} = path_PsychJava;
                pathInserted = 1;
                j = j + 1;
            end
        end

        % If the PsychJava path wasn't inserted, then this must be a new
        % installation, so we append it to the classpath.
        if ~pathInserted
            newFileContents{end + 1, 1} = path_PsychJava;
        end

        % Now compare to see if the new and old classpath are the same.  If
        % they are, then there's no need to do anything.
        updateClasspath = 1;
        if length(fileContents) == length(newFileContents)
            if strcmp(fileContents, newFileContents)
                updateClasspath = 0;
            end
        end

        if updateClasspath
            % Make a backup of the old classpath.
            clear madeBackup;

            [s, w] = copyfile(classpathFile, bakclasspathFile, 'f');

            if s==0
                error(['Could not make a backup copy of Matlab''s JAVA path definition file ''classpath.txt''.\n' ...
                    'The system reports: ', w]);
            end
            madeBackup = 1;

            % Write out the new contents.
            FID = fopen(classpathFile, 'w');
            if FID == -1
                error('Could not open Matlab''s JAVA path definition file ''classpath.txt'' for write access.');
            end
            for i = 1:length(newFileContents)
                fprintf(FID, '%s\n', newFileContents{i});
            end
            fclose(FID);

            fprintf('\n\n');
            disp('*** Matlab''s Static Java classpath definition file modified. You will have to restart Matlab to enable use of the new Java components. ***');
            fprintf('\nPress RETURN or ENTER to confirm you read and understood the above message.\n');
            pause;
        end
    catch
        lerr = psychlasterror;
        fprintf('Could not update the Matlab JAVA classpath.txt file due to the following error:\n');
        fprintf('%s\n\n', lerr.message);
        fprintf('Either you have a very old Matlab version which does not support JAVA or, most\n');
        fprintf('likely, you do not have sufficient access permissions for the Matlab application folder\n');
        fprintf('to change the file %s .\n', classpathFile);
        fprintf('Please ask the system administrator to enable write-access to that file and repeat the\n');        
        fprintf('update procedure.\n');
        fprintf('Alternatively, ask the administrator to add the following line:\n');
        fprintf('%s\n', path_PsychJava);
        fprintf('to the file: %s\n\n', classpathFile);        
        fprintf('If you skip this step, Psychtoolbox will still be mostly functional, \n');
        fprintf('with exception of the Java-based commands ListenChar, CharAvail, GetChar and FlushEvents\n');
        fprintf('on Linux, MacOS-X and M$-Windows in Java mode. For more info see ''help PsychJavaTrouble''.\n\n');
        fprintf('\nPress RETURN or ENTER to confirm you read and understood the above message.\n');
        pause;

        % Restore the old classpath file if necessary.
        if exist('madeBackup', 'var')
            [s, w] = copyfile(bakclasspathFile, classpathFile, 'f');
        end
    end
end % if ~IsOctave

% Check if Screen is functional:
try
    % Check Screen:
    AssertOpenGL;

    % Try to execute online registration routine: This should be fail-safe in case
    % of no network connection.
    fprintf('\n\n');
    PsychtoolboxRegistration(isUpdate, flavor);
    fprintf('\n\n\n');

    % Tell user we're successfully done:
    fprintf('\nDone with post-installation. Psychtoolbox is ready for use.\n\n\n');
    
catch
    fprintf('\n\n');
    if IsOctave & IsWin
        % Probably videocapture dll's and other runtime dll's missing:
        fprintf('Screen() or online registration failed to work under MS-Windows with GNU/Octave-3:\n\n');
        fprintf('Probably the required libARVideo.dll and DSVL.dll libraries are not yet installed on your system.\n\n');
        fprintf('Please type ''help ARVideoCapture'' and follow the displayed installation instructions carefully.\n');
        fprintf('After this one-time setup, the Screen command should work properly.\n\n');
        fprintf('If this has been ruled out as a reason for failure, check the troubleshooting instructions on\n');
        fprintf('our Wiki (Download section and FAQ section, maybe also the Bugs section).\n\n');
    else
        fprintf('Screen() or online registration failed to work for some reason:\n\n');
        fprintf('Check the troubleshooting instructions on our Wiki (Download section \n');
        fprintf('and FAQ section, maybe also the Bugs section).\n\n');
    end
    
    fprintf('Once you manage to fix the problem (simply type ''AssertOpenGL'' to verify\n');
    fprintf('that stuff works now), you do not need to run the installation routine again,\n');
    fprintf('but can start working immediately.\n\n');
    fprintf('However, we kindly ask you to execute the following command after everything works,\n');
    fprintf('so your copy gets registered by us for statistical purpose:\n\n');
    fprintf('PsychtoolboxRegistration(%i, ''%s'');\n\n', isUpdate, flavor);
    fprintf('Thanks! Press RETURN or ENTER to confirm you read and understood the above message.\n');
    pause;
    fprintf('\n\n');
end

% Some goodbye, copyright and getting started blurb...
fprintf('GENERAL LICENSING CONDITIONS:\n');
fprintf('-----------------------------\n\n');
fprintf('Almost all of the material contained in the Psychtoolbox-3 distribution\n');
fprintf('is free software. Most material is covered by the GNU General Public license (GPL).\n');
fprintf('A few internal libraries and components are covered by other free software\n');
fprintf('licenses which we understand to be compatible with the GPL v2, e.g., the GNU LGPL\n');
fprintf('license, or the MIT license used by PortAudio, or they are in the public domain.\n\n');
fprintf('3rd-party components which are freely redistributable due to the authors permissions,\n')
fprintf('but are not neccessarily licensed as free software, can be found in the "PsychContributed"\n');
fprintf('subfolder of the Psychtoolbox distribution, accompanied by their respective licenses.\n\n');
fprintf('Unless otherwise noted for specific components, the GPL license applies:\n');
fprintf('Psychtoolbox is free software; you can redistribute it and/or modify\n');
fprintf('it under the terms of the GNU General Public License as published by\n');
fprintf('the Free Software Foundation; either version 2 of the License, or\n');
fprintf('(at your option) any later version. See the file ''License.txt'' in\n');
fprintf('the Psychtoolbox root folder for exact licensing conditions.\n\n');
fprintf('Your standard Psychtoolbox distribution comes without the source code for\n');
fprintf('the binary plugins (the MEX files). If you want to access the corresponding\n');
fprintf('source code, please type "help UseTheSource" for download instructions.\n\n');
fprintf('BEGINNERS READ THIS:\n');
fprintf('--------------------\n\n');
fprintf('If you are new to the Psychtoolbox, you might try this: \nhelp Psychtoolbox\n\n');
fprintf('Psychtoolbox website:\n');
fprintf('web http://www.psychtoolbox.org -browser\n');
fprintf('\n');
fprintf('Please make sure that you have a look at the PDF file Psychtoolbox3-Slides.pdf\n');
fprintf('in the Psychtoolbox/PsychDocumentation subfolder for an overview of differences\n');
fprintf('between Psychtoolbox-2 and Psychtoolbox-3.\n\n');

fprintf('\nEnjoy!\n\n');

% Clear out everything:
if ~IsOctave & IsWin
    clear all;
end

return;
