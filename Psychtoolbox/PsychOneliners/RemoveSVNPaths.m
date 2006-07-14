% newPathList = RemoveSVNPaths(pathList)
% Removes any .svn paths from the pathList.  If no pathList is specified,
% then the program sets pathList to the result of the 'path' command.  This
% function returns a 'pathsep' delimited list of paths omitting the .svn
% paths.
function newPathList = RemoveSVNPaths(pathList)

% If no pathList was passed to the function we'll just grab the one from
% Matlab.
if nargin ~= 1
    % Grab the path list.
    pathList = path;
end

% Break the path list into individual path elements.
pathElements = strread(pathList, '%s', 'delimiter', pathsep);

% Look at each element from the path.  If it doesn't contain a .svn folder
% then we add it to the end of our new path list.
newPathList = [];
for i = 1:length(pathElements)
    if isempty(strfind(pathElements{i}, '/.svn'))
        newPathList = [newPathList, pathElements{i}, pathsep];
    end
end

% Drop the last path separator if the new path list is non-empty.
if ~isempty(newPathList)
    newPathList = newPathList(1:end-1);
end
