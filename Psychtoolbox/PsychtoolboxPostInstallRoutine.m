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

% Special case handling for different Matlab releases on MS-Windoze:
if IsWin & ~IsOctave
    rc = 0;
    try
        % Remove DLL folders from path:
        rmpath([PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR11\']);
        rmpath([PsychtoolboxRoot 'PsychBasic\MatlabWindowsFilesR2007a\']);
        
        % Is this a Release2007a or later Matlab?
        if ~isempty(strfind(version, '2007')) | ~isempty(strfind(version, '2008')) | ~isempty(strfind(version, '2009')) | ~isempty(strfind(version, '2010'))
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
        rehash pathreset;
        rehash toolboxreset;
        clear WaitSecs;
    catch
        fprintf('WARNING: rehashing the Matlab toolbox cache failed. I may fail and recommend\n');
        fprintf('WARNING: Quitting and restarting Matlab, then retry.\n');
    end
    
    try
        % Try if Screen MEX file works...
        WaitSecs(0.1);
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

% Try to execute online registration routine: This should be fail-safe in case
% of no network connection.
fprintf('\n\n');
PsychtoolboxRegistration(isUpdate, flavor);
fprintf('\n\n\n');

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
            disp('*** Matlab''s Static Java classpath definition file modified. Please restart Matlab to enable use of the new Java components. ***');
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

        % Restore the old classpath file if necessary.
        if exist('madeBackup', 'var')
            [s, w] = copyfile(bakclasspathFile, classpathFile, 'f');
        end
    end
end % if ~IsOctave

% Some goodbye, copyright and getting started blurb...
fprintf('\nDone with post-installation. Psychtoolbox is ready for use.\n');
fprintf('Psychtoolbox is free software; you can redistribute it and/or modify\n');
fprintf('it under the terms of the GNU General Public License as published by\n');
fprintf('the Free Software Foundation; either version 2 of the License, or\n');
fprintf('(at your option) any later version. See the file ''License.txt'' in\n');
fprintf('the Psychtoolbox root folder for exact licensing conditions.\n\n');

fprintf('If you are new to the Psychtoolbox, you might try this: \nhelp Psychtoolbox\n\n');
fprintf('Psychtoolbox website:\n');
fprintf('web http://www.psychtoolbox.org -browser\n');
fprintf('\n');
fprintf('Please make sure that you have a look at the PDF file Psychtoolbox3-Slides.pdf\n');
fprintf('in the Psychtoolbox/PsychDocumentation subfolder for an overview of differences\n');
fprintf('between Psychtoolbox-2 and Psychtoolbox-3.\n\n');

fprintf('\nEnjoy!\n\n');

% Clear out everything:
clear all;

return;
