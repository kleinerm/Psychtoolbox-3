function PsychtoolboxPostInstallRoutine(isUpdate, flavor)
% PsychtoolboxPostInstallRoutine(isUpdate [, flavor])
%
% Psychtoolbox post installation routine. You should not call this
% file directly! This routine is called by DownloadPsychtoolbox,
% or UpdatePsychtoolbox after a successfull download/update of
% Psychtoolbox. The routine performs tasks that are common to
% downloads and updates, so they can share their code/implementation.
%
% As PsychtoolboxPostInstallRoutine itself is downloaded or updated,
% it can contain code specific to each Psychtoolbox revision/release
% to perform special setup procedures for new features, to announce
% important info to the user, whatever...

% History:
% 23/06/2006 Written (MK). Has no real function yet, but soon...

fprintf('Running post-install routine...\n');

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
        end
    catch
        fprintf('Info: Failed to determine flavor of this Psychtoolbox. Not a big deal...\n');
    end
else
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

% Try to execute online registration routine: This should be fail-safe in case
% of no network connection.
PsychtoolboxRegistration(isUpdate, flavor);

% If we're using Matlab on OSX, then add the PsychJava stuff to the static
% Java classpath.
if IsOSX && ~IsOctave
    try
        % Figure out the PsychJava path we need to add to the static Java classpath.
        path_PsychtoolboxVersion = which('PsychtoolboxVersion');
        path_PsychBasic = fileparts(path_PsychtoolboxVersion);
        path_PsychtoolboxRoot = fileparts(path_PsychBasic);
        path_PsychJava = fullfile(path_PsychtoolboxRoot, 'PsychJava');

        % Open up the classpath.txt file and find any PsychJava entries.  If
        % they exist, remove them, and put the current one in the file.  This
        % only allows on PsychJava to be on the path.
        classpathFile = which('classpath.txt');
        fileContents = textread(classpathFile, '%s');
        j = 1;
        newFileContents = {};
        pathInserted = false;
        for i = 1:length(fileContents)
            % Look for the first instance of PsychJava in the classpath and
            % replace it with the new one.  All other instances will be
            % ignored.
            if isempty(findstr('PsychJava', fileContents{i}))
                newFileContents{j, 1} = fileContents{i};
                j = j + 1;
            elseif ~isempty(findstr('PsychJava', fileContents{i})) && ~pathInserted
                newFileContents{j, 1} = path_PsychJava;
                pathInserted = true;
                j = j + 1;
            end
        end

        % If newFileContents is not empty, then proceed to writing out the new file to
        % disk.
        if ~isempty(newFileContents)
            % Now compare to see if the new and old classpath are the same.  If
            % they are, then there's no need to do anything.
            updateClasspath = true;
            if length(fileContents) == length(newFileContents)
                if strcmp(fileContents, newFileContents)
                    updateClasspath = false;
                end
            end

            if updateClasspath
                try
                    % Make a backup of the old classpath.
                    clear madeBackup;
                    [s, w] = system(['cp -f ', classpathFile, ' ', classpathFile, '.bak']);
                    if s
                        error('Could not make a backup of classpath.txt');
                    end
                    madeBackup = true;

                    % Write out the new contents.
                    FID = fopen(classpathFile, 'w');
                    if FID == -1
                        error('Could not open classpath.txt');
                    end
                    for i = 1:length(newFileContents)
                        fprintf(FID, '%s\n', newFileContents{i});
                    end
                    fclose(FID);

                    disp('*** Static Java classpath modified.  Please restart Matlab.');
                catch
                    lerr = psychlasterror;
                    fprintf('The following error occurred trying to modify the static java classpath\n');
                    fprintf('%s\n', lerr.message);

                    % Restore the old classpath file if necessary.
                    if exist('madeBackup', 'var')
                        system(['cp -f ', classpathFile, '.bak ', classpathFile]);
                    end
                end
            end
        else % if ~isempty(newFileContents)
            fprintf('There was a problem reading the file %s\n', classpathFile);
            fprintf('Possibly you do not have permissions to do so or the file does not exist\n');
        end
    catch
        lerr = psychlasterror;
        fprintf('Could not update the classpath due to the following error.\n');
        fprintf('%s\n', lerr.message);
    end
end % if IsOSX && ~IsOctave


fprintf('Done.\n');
return;
