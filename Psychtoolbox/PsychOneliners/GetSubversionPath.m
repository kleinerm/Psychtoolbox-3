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

% History:
% 11/21/06 Written (MK).
% 01/19/09 Update to search in /bin and /usr/bin as well on OS/X.
% 03/10/13 Change search path order to match DownloadPsychtoolbox on OS/X (MK)
% 04/24/13 Move check for /opt/subversion/bin/svn first. Nicolas Cottaris in
%          my lab says this fixes a problem that arose when he installed SVN 1.7.9. (DHB)
% 10/28/13 Add IsLinux where we try out various possible UNIX paths.
%          Maria Olkkonen reports that doing so makes this work properly
%          on her linux system.  (DHB)
% 06/11/18 Change search order for svn executable to account for preferred location
%          on macOS, as provided by XCode command line tools. (MK)
% 10/28/20 Change path search order to deal with latest Apple XCode BS.
%          Also add some hint on how to get svn if needed on macOS.

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

  % Currently, we only know how to check this for Mac OSX and Linux.
  if (IsOSX || IsLinux)
    svnpath = '';

    % /usr/local/bin is our best shot, because that's where
    % HomeBrew will install its svn client:
    if isempty(svnpath) && exist('/usr/local/bin/svn', 'file')
        svnpath = '/usr/local/bin/';
    end

    % XCode would install its svn in /usr/bin/, except the iToys
    % company f$#ked it up as of the Catalina trainwreck:
    if isempty(svnpath) && exist('/usr/bin/svn', 'file')
        svnpath = '/usr/bin/';
    end

    % From here on unlikely fallback locations:
    if isempty(svnpath) && exist('/bin/svn','file')
      svnpath='/bin/';
    end

    if isempty(svnpath) && exist('/opt/local/bin/svn', 'file')
      svnpath = '/opt/local/bin/';
    end

    if isempty(svnpath) && exist('/opt/subversion/bin/svn', 'file')
      svnpath = '/opt/subversion/bin/';
    end
  end
end

% Check that subversion client is installed.
if IsOSX && isempty(svnpath)
    fprintf('The Subversion command line client "svn" is not in its expected\n');
    fprintf('location on your disk. On old macOS versions, please download and install\n');
    fprintf('the most recent Subversion client via typing this into a terminal window:\n');
    fprintf('xcode-select --install\n');
    fprintf('and then run %s again. This will not work on Catalina and later though.\n', mfilename);
    fprintf('On Catalina and later you may be able to install HomeBrew (https://brew.sh)\n');
    fprintf('and then install Subversion via ''brew install subversion''.\n');
    error('Subversion client is missing. Please install it.');
end

return;
