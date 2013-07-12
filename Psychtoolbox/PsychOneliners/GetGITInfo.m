function gitInfo = GetGITInfo(directory)
% gitInfo = GetGITInfo(directory)
%
% Description:
% Retrieves the git information on a specified directory or file.  This is
% essentially a wrapper around the shell command "git describe".
%
% Input:
% directory (string) - Directoryname of interest.
%
% Output:
% gitInfo (struct) - Structure containing the following information:
%   Path
%	Revision
%
%	'gitInfo' will be empty if there is no git info for 'directory'.
%
% 7/11/13  dhb  Wrote it based on GetSVNInfo

if nargin ~= 1
	error('Usage: gitInfo = GetGITInfo(directory)');
end

gitInfo = [];

% Look to see if we can find the git executable on the path.
gitPath = sprintf('%sgit', GetGitPath);
if ~exist(gitPath, 'file')
	fprintf('*** Failed to find git, returning empty.\n');
	return;
end

% Get the git describe info of the specified directory.
curDir = pwd;
cd(directory);
[status, result] = system(sprintf('%s describe --always', gitPath));
cd(curDir);
if status == 0
    gitInfo.Path = directory;
    gitInfo.Revision = result(1:end-1);
end

