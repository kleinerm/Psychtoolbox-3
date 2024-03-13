function latestReleaseData=installPTB(varargin)
% Install Psychtoolbox-3 by fetching zipball from GitHub.com using GitHub's Rest API.
%
%    installPTB                   - Download and install the latest release
%    installPTB('ver', '3.0.17.6')- Download and install a particular version, version number should follow schematic versioning.


% Check if PTB is already installed to MATLAB's Add Ons folder
addonsPath=getAddOnsFolder();
PTBPath=fullfile(addonsPath, "Toolboxes", "Psychtoolbox");
if isfolder(PTBPath)
    error("Psychtoolbox folder already exists within %s. Run this script after deleting the Psychtoolbox folder.",...
        fullfile(addonsPath, "Toolboxes"));
end

% Connect to GitHub.com and retrieve the last 100 release metadata as a JSON archive
releaseRequest = "https://api.github.com/repos/Psychtoolbox-3/Psychtoolbox-3/releases?per_page=100";

% Get the list of all releases from GitHub.com, this archive is sorted
% according to the release data.
releaseJSON = webread(releaseRequest);
checkSemanticVersion = @isSemanticVersion;
latestVersion = releaseJSON{1}.tag_name;

% Set up the parser to parse the input
p=inputParser();
p.CaseSensitive = false;
p.addParameter('ver',latestVersion,checkSemanticVersion);
p.parse(varargin{:});


% check if tag is present in the release archive
didTagMatch=false;
for i=1:length(releaseJSON)
    didTagMatch=strcmp(releaseJSON{i}.tag_name,p.Results.ver);
    if didTagMatch
        break;
    end
end

if ~didTagMatch
    error("Version not found.");
end

% Set release to download, the index i comes from the previous for loop
releaseToDownload=releaseJSON{i};
zipArchiveURL=releaseToDownload.zipball_url;

% Inform the user what you are downloading
if i == 1
    latestreleaseMessage = " (latest release)";
else
    latestreleaseMessage = "";
end
fprintf("Downloading Psychtoolbox-3 version: %s%s\n", releaseToDownload.tag_name,latestreleaseMessage);


% msg="Download URL: "+ zipArchiveURL;
fprintf("Download URL: %s\n", zipArchiveURL);

% Check if the Add Ons folder has write permission
if(~isFolderWritable(addonsPath))
    error("The Add Ons folder is not writable.");
end

% Download the zip archive
zipPath=fullfile(addonsPath, "ptb.zip");
websave(zipPath,zipArchiveURL);
fprintf("Download successful.\n");

% Unzip the zip archive
fNames=unzip(zipPath, addonsPath);

% Move the files in place
% fNames{1} gives the root of the extracted repository
sourcePath=fullfile(fNames{1}, "Psychtoolbox");
destinationPath=fullfile(addonsPath, "Toolboxes");

if(~isFolderWritable(destinationPath))
    error("Trying to install Psyschtoolbox to %s. This folder is not writable", destinationPath);
end
movefile(sourcePath,destinationPath);

% Clean-up remove the zip and the extracted files
rmdir(fNames{1},'s');
delete(zipPath);

% Add the required folders to path
ptbpath=fullfile(destinationPath,"Psychtoolbox");
addpath(genpath(ptbpath));
savepath;

% Say goodbye
fprintf("Installation successful.\n");
end

function isSemanticVersion(versionStr)
% Check if the version number follows major.minor.bugfix.patch
% Example 3.0.7.13
semVerRegex = '^\d+\.\d+\.\d+\.\d+$';
isValid = ~isempty(regexp(versionStr, semVerRegex, 'once'));
if ~isValid
    error("Version number must be of the form major.minor.bug.patch")
end
end


function isWritable = isFolderWritable(folderPath)
    % isFolderWritable Checks if the specified folder is writable.
    % Usage:
    %   isWritable = isFolderWritable('C:\path\to\your\folder')

    % Check if the folder exists
    if ~isfolder(folderPath)
        error("The folder %s does not exist.", folderPath);
    end
    
    % Get folder attributes
    [status, attr] = fileattrib(folderPath);
    
    % Check if the 'Writable' attribute is true
    if status
        isWritable = attr.UserWrite;
    else
        isWritable = false;
    end
end



