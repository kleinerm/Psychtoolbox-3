function svnpath = GetSubversionPath
% svnpath = GetSubversionPath -- Return auto-detected installation path
% for svn client, if any. Return empty string if auto-detection not
% possible. Typical usage is like this:
%
% mysvncommand = [GetSubversionPath 'svn status']; system(mysvncommand);
%
% GetSubversionPath will return the path to be prefixed in front of the svn
% executable. If none can be found, the svn executable will be executed
% without path spec. If it is installed in the system executable search
% path, it will then still work.
%
% The function simply checks if the svn executable is in the Matlab path
% and returns a proper path-spec. If it isn't found in the Matlab path, it
% tries default path locations for OS-X and Windows. If that doesn't work,
% it returns an empty string.
%
% History:
% 11/21/06 Written (MK).

% Check for alternative install location of Subversion:
if IsWin
    % Search for Windows executable in Matlabs path:
    svnpath = which('svn.exe');
else
    % Search for Unix executable in Matlabs path:
    svnpath = which('svn.');
end

% Found one?
if ~isempty(svnpath)
    % Extract basepath and use it:
    svnpath=[fileparts(svnpath) filesep];
else
    % Could not find svn executable in Matlabs path. Check the default
    % install location on OS-X and abort if it isn't there. On M$-Win we
    % simply have to hope that it is in some system dependent search path.

    % Currently, we only know how to check this for Mac OSX.
    if IsOSX
        svnpath='/usr/local/bin/';
        if exist('/usr/local/bin/svn','file')~=2
            % Doesn't exist at expected location - We give up.
            svnpath = '';
        end
    end
end

return;
