function newPathList = RemoveSVNPaths(pathList)
% newPathList = RemoveSVNPaths(pathList)
% Removes any .svn paths from the pathList.  If no pathList is specified,
% then the program sets pathList to the result of the 'path' command.  This
% function returns a 'pathsep' delimited list of paths omitting the .svn
% paths.

% History:
% 14.07.06 Written by Christopher Broussard.
% 25.07.06 Modified to work on M$-Windows and GNU/Octave as well (MK).
% 31.05.09 Adapted to fully work on Octave-3 (MK).
% 30.05.13 Delegate to more general RemoveMatchingPaths (BSH)

% If no pathList was passed to the function we'll just grab the one from
% Matlab.
if nargin ~= 1
    % Grab the path list.
    pathList = path;
end

% use the general path-remover, targeting ".svn"
newPathList = RemoveMatchingPaths(pathList, '.svn');
