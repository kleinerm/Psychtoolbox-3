function DownloadAdditionsForNeuroDebian(targetdirectory, flavor)
% DownloadAdditionsForNeuroDebian(targetdirectory);
% Install missing Matlab or Octave mex files for the NeuroDebian Psychtoolbox.
%
% This function must be called after you have installed octave-psychtoolbox-3
% properly from the NeuroDebian repository (http://neuro.debian.net).
%
% It needs subversion to be installed on your machine (sudo apt-get install subversion).
%
% Then you can call this function from within Matlab, providing the full path
% to a directory. The function will create a new folder PsychtoolboxAddOns inside
% that directory and add it to your path. The add on files will be downloaded
% from the Psychtoolbox main repository and stored there.
%
% After this function successfully completes, your Psychtoolbox should work
% with Matlab on Linux as well.
%
% If you call this function from within Octave, it will download only a
% few additional addon files for the Octave Psychtoolbox. Currently the
% only added file is the Eyelink mex files for SR-Research Eyelink Gazetrackers.
%
% This function should eventually become obsolete when the NeuroDebian
% project adds Matlab Psychtoolbox support to its repository. It is a stop
% gap measure until then.
%

% History:
% 29.09.2011  mk  Written.
%

% NeuroDebian package installed?
if ~exist('/usr/share/octave/site/m/psychtoolbox-3/', 'dir')
  fprintf('This system is missing a octave-psychtoolbox-3 installation from the\n');
  fprintf('NeuroDebian project. Go to http:////neuro.debian.net first, add their\n');
  fprintf('repository to your software management system and then install octave-psychtoolbox-3:\n');
  fprintf('sudo apt-get intall octave-psychtoolbox-3\n');
  fprintf('Then retry this script to get missing proprietary plugins or the Matlab bits.\n');
  error('Required octave-psychtoolbox-3 package from NeuroDebian missing!');
end

% Yes: Add its root path, so we find our M-Functions:
addpath(genpath('/usr/share/octave/site/m/psychtoolbox-3/'));
savepath;

% From here on we should have access to all Psychtoolbox-3 M-Files, but not
% yet to mex files -- those are what we wanna get from Berlios-SVN after all...

%
% Get svn revision number for this octave-psychtoolbox3 installation from the
% NeuroDebian repository:
v = PsychtoolboxVersion ;
rev = strfind (v, 'svn');
v = v(rev+3:end);
rev = sscanf (v, '%i');

% Try to extract the missing Matlab mex files and other proprietary
% stuff from the Psychtoolbox SVN repo. Get the files corresponding
% to the exact revision of the NeuroDebian installation:
if ~exist('IsLinux') || ~IsLinux || isempty(strfind(PsychtoolboxVersion, 'Debian'))
  fprintf('Not sure how you managed to get me to get up to here, but i am confused\n');
  fprintf('Anyway, this is not a correctly configured Linux system with NeuroDebian psychtoolbox.\n');
  fprintf('Giving up!\n');
  error('This function is only supported on Linux with an already installed NeuroDebian Psychtoolbox.');
end

if nargin < 1 || isempty(targetdirectory)
  error('Required "targetdirectory" parameter is missing. You must tell me where to install the additional files!');
end

% Strip trailing fileseperator, if any:
if targetdirectory(end) == filesep
    targetdirectory = targetdirectory(1:end-1);
end

% Build target revision string:
targetRevision = ['-r ' num2str(rev) ' '];

% Set flavor defaults and synonyms
if nargin < 4
    flavor = [];
end

if isempty(flavor)
    flavor='beta';
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
        fprintf('The "stable" flavor is no longer available, it has been renamed to "unsupported".\n');
        fprintf('If you really want to use the former "stable" flavor, please retry the download\n');
        fprintf('under the new name "unsupported".\n\n');
        error('Flavor "stable" requested. This is no longer available.');
    case 'unsupported'
        % Very bad choice! Give user a chance to reconsider...
        fprintf('\n\n\nYou request download of the "unsupported" flavor of Psychtoolbox.\n');
        fprintf('Use of the "unsupported" flavor is strongly discouraged! It is outdated and contains\n');
        fprintf('many bugs and deficiencies that have been fixed in the recommended "beta" flavor years ago.\n');
        fprintf('"unsupported" is no longer maintained and you will not get any support if you have problems with it.\n');
        fprintf('Please choose "beta" unless you have very good reasons not to do so.\n\n');
        fprintf('If you answer "no" to the following question, i will download the recommended "beta" flavor instead.\n');
        answer=input('Do you want to continue download of "unsupported" flavor despite the warnings (yes or no)? ','s');
        if ~strcmpi(answer,'yes') && ~strcmpi(answer,'y')
            flavor = 'beta';
            fprintf('Download of "unsupported" flavor cancelled, will download recommended "beta" flavor instead...\n');
        else
            fprintf('Download of "unsupported" flavor proceeds. You are in for quite a bit of pain...\n');            
        end

        fprintf('\n\nPress any key to continue...\n');
        pause;
        
    otherwise
        fprintf('\n\n\nHmm, requested flavor is the unusual flavor: %s\n',flavor);
        fprintf('Either you request something exotic, or you made a typo?\n');
        fprintf('We will see. If you get an error, this might be the first thing to check.\n');
        fprintf('Press any key to continue...\n');
        pause;
end

if strcmp(flavor, 'beta')
    % Check if this is Matlab of version prior to V 6.5:
    v = ver('matlab');
    if ~isempty(v)
        v = v(1).Version; v = sscanf(v, '%i.%i.%i');
        if (v(1) < 6) | ((v(1) == 6) & (v(2) < 5)) %#ok<AND2,OR2>
            % Matlab version < 6.5 detected. This is no longer
            % supported by current PTB beta. Redirect to the last
            % functional PTB for such ancient Matlab's:
            flavor = 'Psychtoolbox-3.0.8-PreMatlab6.5';
            fprintf('\n\n\nYou request download of the "beta" flavor of Psychtoolbox.\n');
            fprintf('The "beta" flavor is no longer available for your version of Matlab.\n');
            fprintf('Current "beta" only works on Matlab Version 6.5 or later.\n');
            fprintf('I will download "%s" instead, which is the last working\n', flavor);
            fprintf('version of Psychtoolbox for older Matlab releases. Please consider\n');
            fprintf('upgrading to a recent Matlab version or switching to GNU/Octave 3.2.x. Both\n');
            fprintf('should provide better support, performance and a richer feature set.\n\n');
            fprintf('Running on your ancient Matlab should work, but is not supported\n');
            fprintf('anymore. If you run into any problems or bugs, you are on your own.\n');

            fprintf('\n\nPress any key to continue installation or abort now via CTRL+C...\n\n\n');
            pause;
        end
    end
end

fprintf('DownloadAdditionsForNeuroDebian(''%s'',''%s'')\n',targetdirectory, flavor);
fprintf('Requested flavor is: %s\n',flavor);
fprintf('Requested location for the Psychtoolbox folder is inside: %s\n',targetdirectory);
fprintf('\n');

% Search for Unix executable in path:
svnpath = which('svn.');

% Found one?
if ~isempty(svnpath)
    % Extract basepath and use it:
    svnpath=[fileparts(svnpath) filesep];
end

if ~isempty(svnpath)
    fprintf('Will use the svn client which is located in this folder: %s\n', svnpath);
end

% Do we have sufficient privileges to install at the requested location?
p='Psychtoolbox123test';
[success,m,mm]=mkdir(targetdirectory,p);
if success
    rmdir(fullfile(targetdirectory,p));
else
    fprintf('Write permission test in folder %s failed.\n', targetdirectory);
    if strcmp(m,'Permission denied')
            fprintf([
            'Sorry. You would need administrator privileges to install the \n'...
            'Psychtoolbox into the ''%s'' folder. Please rerun the script, choosing \n'...
            'a location where you have write permission, or ask a user with administrator \n'...
            'privileges to run this function for you.\n\n'],targetdirectory);
            error('Need administrator privileges for requested installation into folder: %s.',targetdirectory);
    else
        error(mm,m);
    end
end
fprintf('Good. Your privileges suffice for the requested installation into folder %s.\n\n',targetdirectory);

p=fullfile(targetdirectory,'PsychtoolboxAddons');

% Create quoted version of 'p'ath, so blanks in path are handled properly:
pt = strcat('"',p,'"');

% What do we need from Berlios?
delpattern = [];

if ~IsOctave
  % The Matlab mex files:
  if IsLinux(1)
    sourcefolder = 'PsychBasic/';
    delpattern = '*.mexglx';
  else
    sourcefolder = 'PsychBasic/';
    delpattern = '*.mexa64';
  end

  % Build final checkout command string:
  checkoutcommand = [svnpath 'svn export --force -N ' targetRevision ' https://svn.berlios.de/svnroot/repos/osxptb/' flavor '/Psychtoolbox/' sourcefolder ' ' pt];
else
  % Additional Octave mex files:
  % Only Eyelink so far...
  if IsLinux(1)
    sourcefolder = ['PsychBasic/Octave3LinuxFiles64/Eyelink.mex'];
  else
    sourcefolder = ['PsychBasic/Octave3LinuxFiles/Eyelink.mex'];
  end

  if exist([targetdirectory '/PsychtoolboxAddons/'], 'dir')
    system(['rm -r ' targetdirectory '/PsychtoolboxAddons/']);
  end
  [success,m,mm]=mkdir(targetdirectory, 'PsychtoolboxAddons/');
  pt = strcat('"',p,'/Eyelink.mex"');

  % Build final checkout command string:
  checkoutcommand = [svnpath 'svn export --force -N ' targetRevision ' https://svn.berlios.de/svnroot/repos/osxptb/' flavor '/Psychtoolbox/' sourcefolder ' ' pt];
end

fprintf('The following EXPORT command asks the Subversion client to \ndownload the few additional bits of Psychtoolbox:\n');
fprintf('%s\n',checkoutcommand);
fprintf('Downloading. It''s a few megabytes, which can take a bit of time. \nAlas there may be no output to this window to indicate progress until the download is complete. \nPlease be patient ...\n');
fprintf('If you see some message asking something like "accept certificate (p)ermanently, (t)emporarily? etc."\n');
fprintf('then please press the p key on your keyboard, possibly followed by pressing the ENTER key.\n\n');
err = system(checkoutcommand);
result = 'For reason, see output above.';

if err
    fprintf('Sorry, the download command "EXPORT" failed with error code %d: \n',err);
    fprintf('%s\n',result);
    fprintf('The download failure might be due to temporary network or server problems. You may want to try again in a\n');
    fprintf('few minutes. It could also be that the subversion client was not (properly) installed.\n');
    error('Download failed.');
end
fprintf('Download succeeded!\n\n');

if ~IsOctave
  % Remove all m-files and unrelated mex files:
  pt = strcat('',p,'/*.m');
  system(['rm ' pt ]);
  system(['rm ' p '/*.mexmac']);
  system(['rm ' p '/*.mexmaci']);
  if ~isempty(delpattern)
    system(['rm ' p '/' delpattern]);
  end
end

% Add Psychtoolbox to MATLAB / OCTAVE path
fprintf('Now adding the new Psychtoolbox add-on folder (and all its subfolders) to your MATLAB / OCTAVE path.\n');
pp=genpath(p);
addpath(pp);

if exist('savepath')
   err=savepath;
else
   err=path2rc;
end

if err
    fprintf('SAVEPATH failed. Psychtoolbox is now already installed and configured for use on your Computer,\n');
    fprintf('but i could not save the updated MATLAB / OCTAVE path, probably due to insufficient permissions.\n');
    fprintf('You will either need to fix this manually via use of the path-browser (Menu: File -> Set Path),\n');
    fprintf('or by manual invocation of the savepath command (See help savepath). The third option is, of course,\n');
    fprintf('to add the path to the Psychtoolbox folder and all of its subfolders whenever you restart MATLAB / OCTAVE.\n\n\n');
else 
    fprintf('Success.\n\n');
end

% Matlab specific setup:
if ~IsOctave
    % Check if this is Matlab of version prior to V 6.5:
    v = ver('matlab');
    if ~isempty(v)
        v = v(1).Version; v = sscanf(v, '%i.%i.%i');
        if (v(1) < 6) | ((v(1) == 6) & (v(2) < 5)) %#ok<AND2,OR2>
            % Matlab version < 6.5 detected. This is no longer
            % supported.
            fprintf('\n\nYou are using a Matlab version older than Version 6.5.\n');
            fprintf('The current "beta" flavor is no longer compatible with your version of Matlab.\n');
            fprintf('Current "beta" only works on Matlab Version 6.5 or later.\n\n');
            fprintf('I will try to finish setup, but most functions will not work for you.\n');
            fprintf('Please run the DownloadPsychtoolbox downloader now to download an outdated,\n');
            fprintf('but functional older version of Psychtoolbox for your Matlab setup or to\n');
            fprintf('receive further instructions.\n');
            fprintf('\n\nPress any key to continue after you have read and understood above message completely.\n\n');
            pause;
        end
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
                newFileContents{j, 1} = fileContents{i}; %#ok<AGROW>
                j = j + 1;
            elseif ~isempty(findstr('PsychJava', fileContents{i})) & ~pathInserted %#ok<AND2>
                newFileContents{j, 1} = path_PsychJava; %#ok<AGROW>
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
            madeBackup = 1; %#ok<NASGU>

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
        fprintf('or file itself to change the file %s .\n', classpathFile);
        fprintf('Please ask the system administrator to enable write-access to that file and its\n');        
        fprintf('containing folder and then repeat the update procedure.\n');
        fprintf('Alternatively, ask the administrator to add the following line:\n');
        fprintf('%s\n', path_PsychJava);
        fprintf('to the file: %s\n\n', classpathFile);        
        fprintf('If you skip this step, Psychtoolbox will still be mostly functional, \n');
        fprintf('but the Java-based commands ListenChar, CharAvail, GetChar and FlushEvents\n');
        fprintf('on Linux, MacOS-X and M$-Windows in Java mode will work less efficiently.\n');
        fprintf('For more info see ''help PsychJavaTrouble''.\n\n');
        fprintf('\nPress RETURN or ENTER to confirm you read and understood the above message.\n');
        pause;

        % Restore the old classpath file if necessary.
        if exist('madeBackup', 'var')
            [s, w] = copyfile(bakclasspathFile, classpathFile, 'f'); %#ok<NASGU>
        end
    end
end % if ~IsOctave

% Some goodbye, copyright and getting started blurb...
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
fprintf('e.g., the PsychCV() function are currently covered by the standard GPL v2 license and\n');
fprintf('cannot be used with Matlab. These are only available for use with GNU/Octave.\n\n');

fprintf('Please read the license text and copyright info in the Psychtoolbox file\n');
fprintf('License.txt carefully before you use or redistribute Psychtoolbox-3.\n');
fprintf('Use of Psychtoolbox-3 components implies that you have read, understood and accepted\n');
fprintf('the licensing conditions.\n\n');
fprintf('However, in a nutshell, if you just use Psychtoolbox for your research, our licenses\n');
fprintf('don''t restrict you in any practically relevant way. Commercial users, developers or\n');
fprintf('redistributors should make sure they understood the licenses for the components they use.\n');
fprintf('If in doubt, contact one of the Psychtoolbox developers, or the original authors of the\n');
fprintf('components you want to use, modify, merge or redistribute with other software.\n\n');
fprintf('Your standard Psychtoolbox distribution comes without the source code for\n');
fprintf('the binary plugins (the MEX files). If you want to access the corresponding\n');
fprintf('source code, please type "help UseTheSource" for specific download instructions.\n\n');
fprintf('BEGINNERS READ THIS:\n');
fprintf('--------------------\n\n');
fprintf('If you are new to the Psychtoolbox, you might try this: \nhelp Psychtoolbox\n\n');
fprintf('Psychtoolbox website:\n');
fprintf('web http://www.psychtoolbox.org -browser\n');
fprintf('\n');
fprintf('Please make sure that you have a look at the PDF file Psychtoolbox3-Slides.pdf\n');
fprintf('in the Psychtoolbox/PsychDocumentation subfolder for an overview of differences\n');
fprintf('between Psychtoolbox-2 and Psychtoolbox-3 and proper use of basic features. That\n');
fprintf('folder contains various additional helpful information for use of Psychtoolbox.\n\n');
fprintf('\n');
fprintf('Please also familiarize yourself with the demos contained in the PsychDemos subfolder\n');
fprintf('and its subfolders. They show best practices for many common tasks and are generally\n');
fprintf('well documented.\n');

fprintf('\nEnjoy!\n\n');

end
