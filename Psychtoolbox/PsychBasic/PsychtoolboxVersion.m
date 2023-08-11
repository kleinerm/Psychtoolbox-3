function [versionString, versionStructure] = PsychtoolboxVersion
%
% [versionString, versionStructure] = PsychtoolboxVersion
%
% Return a string identifying this release of the Psychtoolbox.
% The first three numbers identify the base version of Psychtoolbox:
%
% - Leftmost (major version): Increments indicate a disruptive change in
% the feature set, application programming interfaces or core design of the
% software, by abrupt introduction of drastic design changes. This should
% never happen, as this would mean a completely new product, entirely
% incompatible with the old software.
%
% - Middle (minor version): Increments indicate very significant
% enhancements or changes in functionality. This would for example happen
% if there would be backwards incompatible changes to the programming
% interface or functionality which may require code adjustments in user
% code or even choosing different hardware or operating systems to run it.
% This will usually only happen every couple of years at most.
%
% - Rightmost (patch level): A counter to distinguish multiple releases
% having the same leftmost and middle version numbers. It increments if we
% cancel support for platforms (Matlab/Octave versions, operating system
% versions, processor architectures etc.). This happens occasionally.
%
% Numeric values of the three integer fields contained in versionString are
% available in fields of the second return argument, "versionStructure".
%
% If Psychtoolbox was installed using Neurodebian or GIT, this source
% is available in the field 'Flavor'.
%
% Using SVN, the field 'Flavor' defines the subtype of Psychtoolbox being used:
%
% * beta: The term 'current' is a synonym for 'beta'. Beta releases are the
% only releases we provide at this point. 'Beta' is a historical name, not
% anything is really much of beta quality about them, they are considered
% the most stable and trustworthy releases.
%
% * trunk: Development prototypes, for testing and debugging by developers
% and really adventureous users, not for production use!
%
% * Psychtoolbox-x.y.z: Old, no longer supported Psychtoolbox versions.
%
% * Debian package: A Psychtoolbox provided by GNU/Debian, Ubuntu or NeuroDebian.
%
% The revision number and the provided URL allows you to visit the developer
% website in the Internet and get direct access to all development logs
% regarding your working copy of Psychtoolbox.
%
% Be aware that execution of the PsychtoolboxVersion command can take a lot
% of time (in the order of multiple seconds to 1 minute).
%
% Most Psychtoolbox mex files now provide a built-in 'Version' command
% which returns version for themselves.  The version string for the
% built-in version numbers contains a fourth numeric field named "build".
% The build number is a unique serial number.  Mex files distinguished only
% by build numbers were compiled from identical C source files.
%
% _________________________________________________________________________
%
% see also: Screen('Version')

%   2/9/01     awi      added fullfile command for platform-independent pathname
%   6/29/02    dgp      Use new PsychtoolboxRoot function to cope with user-changed folder names.
%   7/12/04    awi      ****** OS X-specific fork from the OS 9 version *******
%                       Noted mex file versioning.
%   7/26/04    awi      Partitioned help and added OS X section.  Enhanced
%                       OS9+Windows section.
%   10/4/05    awi      Note here that dgp changed "IsWindows" to "IsWin" at unknown date prior
%                       between 7/26/04 and 10/4/05.
%
%   5/5/06     mk       Tries to query info from Subversion and displays info about last date
%                       of change, SVN revision and flavor. This code is pretty experimental and
%                       probably also somewhat fragile. And its sloooow.
%   9/17/06    mk       Improvements to parser: We try harder to get the flavor info.
%   10/31/11   mk       Update for our new hoster GoogleCode.
%   04/30/12   mk       Kill MacOS-9 support.
%   05/27/12   mk       Switch over to GitHub hosting.
%   08/06/14   mk       Integrate (Neuro)Debian versioning support. Cleanups.
%   10/28/20   mk       Add SVN support via Matlabs SVNKit. Clarify versioning scheme.
%   11/04/20   ia       Add support for git or manual install methods

global Psychtoolbox

if ~isfield(Psychtoolbox,'version')
    Psychtoolbox.version.major=0;
    Psychtoolbox.version.minor=0;
    Psychtoolbox.version.point=0;
    Psychtoolbox.version.string='';
    Psychtoolbox.version.flavor='';
    Psychtoolbox.version.revision=0;
    Psychtoolbox.version.revstring='';
    Psychtoolbox.version.websvn = 'https://github.com/Psychtoolbox-3/Psychtoolbox-3';

    file=fullfile(PsychtoolboxRoot,'Contents.m');
    fileinfo = dir(file); % get the filesystem date of this file
    f=fopen(file,'r');
    fgetl(f);
    s=fgetl(f);
    fclose(f);
    [cvv,count,errmsg,n]=sscanf(s,'%% Version %d.%d.%d',3);
    ss=s(n:end);
    contentsdate=ss(min(find(ss-' ')):end); %#ok<MXFND>
    Psychtoolbox.version.major=cvv(1);
    Psychtoolbox.version.minor=cvv(2);
    Psychtoolbox.version.point=cvv(3);
    
    gitfolder = PsychtoolboxRoot();
    i=find(filesep==gitfolder);
    gitfolder = [gitfolder(1:i(end-1)) '.git'];
    if exist(gitfolder ,'dir')
        isGIT = true;
    else
        isGIT = false;
    end
    
    if exist([PsychtoolboxRoot '.svn'],'dir')
        isSVN = true;
    else
        isSVN = false;
    end

    if any(strcmp(PsychtoolboxRoot, {'/usr/share/octave/site/m/psychtoolbox-3/', ...
        '/usr/share/matlab/site/m/psychtoolbox-3/', '/usr/share/psychtoolbox-3/'}))
        % It is a Debian version of the package
        Psychtoolbox.version.flavor = 'Debian package';
        [status, result] = system('zcat /usr/share/doc/psychtoolbox-3-common/changelog.Debian.gz| head -1 | sed -e "s/).*/)/g"');
        if status == 0
            Psychtoolbox.version.revstring = result(1:end-1);
        else
            Psychtoolbox.version.revstring = 'WARNING: failed to obtain Debian revision';
        end

        if IsOctave
            infourl = 'http://neuro.debian.net/pkgs/octave-psychtoolbox-3.html';
        else
            infourl = 'http://neuro.debian.net/pkgs/matlab-psychtoolbox-3.html';
        end

        % Build final version string:
        Psychtoolbox.version.string = sprintf('%d.%d.%d - Flavor: %s - %s\nFor more info visit:\n%s', Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point, ...
            Psychtoolbox.version.flavor, Psychtoolbox.version.revstring, infourl);

        % Old: Retrieve the date of the Debian release:
        % Does not work anymore, as date is no longer part of upstream release tags.
        % Use contentsdate from Psychtoolbox main help file instead:
        Psychtoolbox.date = contentsdate; % sscanf(result, 'psychtoolbox-3 (%*d.%*d.%*d.%d.%*s');
    elseif isGIT == true
        %assume a GIT install
        gitstatus = GetGITInfo(PsychtoolboxRoot);
        if ~isempty(gitstatus)
            Psychtoolbox.version.flavor = 'GIT Branch';
            Psychtoolbox.version.revision = gitstatus.Revision;
            Psychtoolbox.version.revstring = gitstatus.Describe;
            [r1,r2] = regexp(gitstatus.RemoteRepository{1},'http[^\s]+');
            if ~isempty(r1)
                remoteURL = gitstatus.RemoteRepository{1}(r1:r2);
            else 
                remoteURL = '';
            end
            % Build final version string:
            Psychtoolbox.version.string = sprintf('%d.%d.%d - Flavor: %s - Commit %s\nFor more info visit:\n%s', ...
                Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point, ...
                Psychtoolbox.version.flavor, Psychtoolbox.version.revision, remoteURL);

            % Cell index of LastCommit where date is stored varies:
            for ds=1:numel(gitstatus.LastCommit)
                if ~isempty(strfind(gitstatus.LastCommit{ds}, 'Date'))
                    break;
                end
            end

            date = regexp(gitstatus.LastCommit{ds},'Date:\s+(?<day>\w{3})\s(?<month>\w{3})\s+(?<dayn>\d+)\s(?<time>\d\d\:\d\d:\d\d)\s(?<year>\d{4})','names');
            Psychtoolbox.date = sprintf('%s-%s-%s', date.dayn, date.month, date.year);
        end
    elseif isSVN == false
        % assume a manual install
        Psychtoolbox.version.flavor = 'Manual Install';
        Psychtoolbox.date = fileinfo.date;
        Psychtoolbox.version.string=sprintf('%d.%d.%d - Flavor: %s, %s', ...
        Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point,...
        Psychtoolbox.version.flavor,Psychtoolbox.date);
    else
        if ~IsOctave && ~verLessThan('matlab', '8.4')
            % R2014b and later contain a Java SVN implementation, so lets use
            % that to spare the user from having to install a separate svn
            % command line client:

            % Get svn_client object from Matlab's Java implementation:
            svn_client_manager = org.tmatesoft.svn.core.wc.SVNClientManager.newInstance;
            svn_client = svn_client_manager.getWCClient();
            try
                svninfo = svn_client.doInfo(java.io.File(PsychtoolboxRoot()), org.tmatesoft.svn.core.wc.SVNRevision.WORKING);

                % Get revision:
                Psychtoolbox.version.revision = svninfo.getCommittedRevision().getNumber();
                Psychtoolbox.version.revstring = sprintf('Corresponds to SVN Revision %d', Psychtoolbox.version.revision);

                % Get flavor:
                result = char(svninfo.getURL().getPath());

                % First test for end-user branch:
                marker = '/Psychtoolbox-3/Psychtoolbox-3/branches/';
                startdel = strfind(result, marker) + length(marker);

                if isempty(startdel)
                    % Nope: Search for developer branch aka 'trunk' aka 'master':
                    marker = '/Psychtoolbox-3/Psychtoolbox-3/';
                    startdel = strfind(result, marker) + length(marker);
                end

                findel = min(strfind(result(startdel:length(result)), '/Psychtoolbox')) + startdel - 2;
                Psychtoolbox.version.flavor = result(startdel:findel);

                % Get date:
                date = svninfo.getCommittedDate();
                Psychtoolbox.date = sprintf('%d-%02d-%02d', 1900 + date.getYear(), 1 + date.getMonth(), date.getDate());

                % SVN status query does not yet work, as it won't detect all
                % modifications to the working copy, but only modifications to
                % the root folder itself, iow. none at all in almost all cases.
                % TODO: Would need to use the doStatus() method which recurses
                % over the whole working copy and calls a ISVNStatusHandler for
                % each modified item -- Something to figure out, if such
                % handlers would be doable easily at all from Matlab...
                svn_statusclient = svn_client_manager.getStatusClient();
                svnstatus = svn_statusclient.doStatus(java.io.File(PsychtoolboxRoot()), false);
                if svnstatus.getContentsStatus() == org.tmatesoft.svn.core.wc.SVNStatusType.STATUS_MODIFIED
                    Psychtoolbox.version.revstring = sprintf('%s but is *locally modified* !', Psychtoolbox.version.revstring);
                end

                % Build final version string:
                Psychtoolbox.version.string = sprintf('%d.%d.%d - Flavor: %s - %s\nFor more info visit:\n%s', Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point, ...
                                                        Psychtoolbox.version.flavor, Psychtoolbox.version.revstring, Psychtoolbox.version.websvn);
            catch
                fprintf('PsychtoolboxVersion: WARNING - Could not query additional version information from SVN.\n');
                Psychtoolbox.version.flavor = 'Unknown';
                Psychtoolbox.date = fileinfo.date;
                Psychtoolbox.version.string=sprintf('%d.%d.%d - Flavor: %s, %s', ...
                Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point,...
                Psychtoolbox.version.flavor,Psychtoolbox.date);
            end
        else
            % Fallback: Additional parser code for SVN information. This is slooow!
            svncmdpath = GetSubversionPath;

            % Find revision string for Psychtoolbox that defines the SVN revision
            % to which this working copy corresponds:
            if ~IsWin
                [status , result] = system([svncmdpath 'svnversion -c ' PsychtoolboxRoot]);
            else
                [status , result] = dos([svncmdpath 'svnversion -c ' PsychtoolboxRoot]);
            end

            if status==0
                % Parse output of svnversion: Find revision number of the working copy.
                colpos=strfind(result, ':');
                if isempty(colpos)
                    Psychtoolbox.version.revision=sscanf(result, '%d',1);
                else
                    cvv = sscanf(result, '%d:%d',2);
                    Psychtoolbox.version.revision=cvv(2);
                end
                if isempty(strfind(result, 'M'))
                    Psychtoolbox.version.revstring = sprintf('Corresponds to SVN Revision %d', Psychtoolbox.version.revision);
                else
                    Psychtoolbox.version.revstring = sprintf('Corresponds to SVN Revision %d but is *locally modified* !', Psychtoolbox.version.revision);
                end

                % Ok, now find the flavor and such...
                if ~IsWin
                    [status , result] = system([svncmdpath 'svn info --xml ' PsychtoolboxRoot]); %#ok<*ASGLU>
                else
                    [status , result] = dos([svncmdpath 'svn info --xml ' PsychtoolboxRoot]);
                end

                % First test for end-user branch:
                marker = '/github.com/Psychtoolbox-3/Psychtoolbox-3/branches/';
                startdel = strfind(result, marker) + length(marker);

                if isempty(startdel)
                    % Nope: Search for developer branch aka 'trunk' aka 'master':
                    marker = '/github.com/Psychtoolbox-3/Psychtoolbox-3/';
                    startdel = strfind(result, marker) + length(marker);
                end

                if isempty(startdel)
                    % Nope: Retry with a different query for older svn clients:
                    if ~IsWin
                        [status , result] = system([svncmdpath 'svn info ' PsychtoolboxRoot]);
                    else
                        [status , result] = dos([svncmdpath 'svn info ' PsychtoolboxRoot]);
                    end

                    % Retry first test for end-user branch:
                    marker = '/github.com/Psychtoolbox-3/Psychtoolbox-3/branches/';
                    startdel = strfind(result, marker) + length(marker);

                    if isempty(startdel)
                        % Nope: Retry search for developer branch aka 'trunk' aka 'master':
                        marker = '/github.com/Psychtoolbox-3/Psychtoolbox-3/';
                        startdel = strfind(result, marker) + length(marker);
                    end
                end

                findel = min(strfind(result(startdel:length(result)), '/Psychtoolbox')) + startdel - 2;
                Psychtoolbox.version.flavor = result(startdel:findel);

                % Retrieve the date of last commit:
                startdel = strfind(result, '<date>') + length('<date>');
                findel = max(strfind(result, 'T') - 1);
                Psychtoolbox.date = result(startdel:findel);

                % Build final version string:
                Psychtoolbox.version.string = sprintf('%d.%d.%d - Flavor: %s - %s\nFor more info visit:\n%s', Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point, ...
                                                      Psychtoolbox.version.flavor, Psychtoolbox.version.revstring, Psychtoolbox.version.websvn);
            else
                % Fallback path if svn commands fail for some reason. Output as much as we can.
                fprintf('PsychtoolboxVersion: WARNING - Could not query additional version information from SVN -- svn tools not properly installed?!?\n');
                Psychtoolbox.version.string=sprintf('%d.%d.%d', Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point);
                Psychtoolbox.date=contentsdate;
            end
        end
    end
end

versionString=Psychtoolbox.version.string;
versionStructure=Psychtoolbox.version;
