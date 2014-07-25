function AddToMatlabPathDynamically(directory)
% AddToMatlabPathDynamically(directory)
%
% Add the directory and its subdirectories to Matlab's path, dynamically,
% and then strip standard Brainard lab temporary paths.
% 
% Usefor for putting routines onto path that are specifict to a particular
% project, without them staying around and clogging up the name space.
%
% Typical usages:
% a) When getting version info
%   exp.mFileName = mfilename;
%   [exp.versionInfo,exp.codeDir] = GetAllVersionInfo(exp.mFileName);
%   AddToMatlabPathDynamically(exp.codeDir);
%
% b) Direct call
%   AddToMatlabPathDynamically( fileparts(which(mfilename))); 
%
% 7/12/13  dhb  Wrote it.

%% Dynamically add the program code to the path if it isn't already on it.
if isempty(strfind(path, directory))
	fprintf('- Adding %s dynamically to the path...', directory);
	addpath(RemoveTMPPaths(genpath(directory),false), '-end');
	fprintf('Done\n');
end
