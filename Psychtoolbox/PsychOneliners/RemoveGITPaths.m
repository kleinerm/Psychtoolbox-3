function newPathList = RemoveGITPaths(pathList)
% newPathList = RemoveGITPaths(pathList)
% Removes any .git paths from the pathList.  If no pathList is specified,
% then the program sets pathList to the result of the 'path' command.  This
% function returns a 'pathsep' delimited list of paths omitting the .git
% paths.

% History:
% 30.05.13 Made .git version from RemoveSVNPaths (DHB).

% If no pathList was passed to the function we'll just grab the one from
% Matlab.
if nargin ~= 1
    % Grab the path list.
    pathList = path;
end

try
    % We do the .git path removal in a try-catch block, because some of the
    % functions used inside this block are not available in Matlab-5 and
    % GNU/Octave. Our catch - block provides fail-safe behaviour for that
    % case.
    
    % Break the path list into individual path elements.
    if IsOctave
        pathElements = strsplit(pathList,pathsep,true);
    else
        pathElements = textscan(pathList, '%s', 'delimiter', pathsep);
        pathElements = pathElements{1}.';
    end
    
    % Look at each element from the path.  If it doesn't contain a .git folder
    % then we add it to the end of our new path list.
    qNotGIT = cellfun(@isempty,strfind(pathElements,[filesep '.git']));
    pathElements = pathElements(qNotGIT);
    
    if ~isempty(pathElements)
        % generate new pathList
        pathElements = [pathElements; repmat({pathsep},1,length(pathElements))];
        newPathList  = [pathElements{:}];
        
        % drop last separator
        newPathList(end) = [];
    end
catch
    % Fallback behaviour: We fail-safe by simply returning the unmodified
    % pathList. No .git paths removed, but the whole beast is still
    % functional.
    newPathList = pathList;
end
