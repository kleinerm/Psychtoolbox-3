function newPathList = RemoveSVNPaths(pathList)
% newPathList = RemoveSVNPaths(pathList)
% Removes any .svn paths from the pathList.  If no pathList is specified,
% then the program sets pathList to the result of the 'path' command.  This
% function returns a 'pathsep' delimited list of paths omitting the .svn
% paths.

% History:
% 14.07.06 Written by Christopher Broussard.
% 25.07.06 Modified to work on M$-Windows and GNU/Octave as well (MK).

% If no pathList was passed to the function we'll just grab the one from
% Matlab.
if nargin ~= 1
    % Grab the path list.
    pathList = path;
end

try
    % We do the .svn path removal in a try-catch block, because some of the
    % functions used inside this block are not available in Matlab-5 and
    % GNU/Octave. Our catch - block provides fail-safe behaviour for that
    % case.
    
    % Break the path list into individual path elements.
    pathElements = strread(pathList, '%s', 'delimiter', pathsep);

    % Look at each element from the path.  If it doesn't contain a .svn folder
    % then we add it to the end of our new path list.
    newPathList = [];
    for i = 1:length(pathElements)
        if isempty(findstr(pathElements{i}, [filesep '.svn']))
            newPathList = [newPathList, pathElements{i}, pathsep];
        end
    end

    % Drop the last path separator if the new path list is non-empty.
    if ~isempty(newPathList)
        newPathList = newPathList(1:end-1);
    end
catch
    % Fallback behaviour: We fail-safe by simply returning the unmodified
    % pathList. No .svn paths removed, but the whole beast is still
    % functional.
    newPathList = pathList;
end
