function [versionString, versionStructure]=PsychtoolboxVersion
% OS X, Windows, Linux: ___________________________________________________
% 
% [versionString, versionStructure]=PsychtoolboxVersion
%
% Return a string identifying this release of the Psychtoolbox.
% The first three numbers identify the base version of Psychtoolbox:
%
% ´ Leftmost: increments indicate a significant change in the feature
% set, either through accumulated progress over time or abrupt introduction
% of significant new features.
%
% ´ Middle: Even numbers designate a "stable" release. The objective for
% even number releases is that the software should run stably, as opposed
% to introduction of new features. An even number is not a guarantee of
% stability, but an expression of intent.  Odd numbers indicate a
% "developer" release.  Odd number releases are incomplete, the software is
% made available for the purpose of public collaboration in development.
%
% ´ Rightmost: A counter to distinguish multiple releases having the same
% leftmost and middle version numbers.
%
% Numeric values of the three integer fields contained in versionString are
% available in fields of the second return argument, "versionStructure".
%
% The field 'Flavor' defines the subtype of Psychtoolbox being used:
% * beta: An experimental release that is already tested by the developers,
% but not yet sufficiently tested or proven in the field. Beta releases
% contain lots of new and experimental features that may be useful to you
% but that may change slightly in behaviour or syntax in the final release,
% making it necessary for you to adapt your code after a software update.
% Beta releases are known to be imperfect and fixing bugs in them is not a
% high priority.  The term 'current' is a synonym for 'beta'.
%
% * stable: A release with the intention of being well-tested and reliable.
% Fixing bugs found in stable releases has a high priority and syntax or
% behaviour of features in a stable release is not likely to change. Code
% written against a stable release should work after an update without the
% need for you to modify anything.
%
% The revision number and the provided URL allows you to visit the developer
% website in the Internet and get direct access to all development logs
% regarding your working copy of Psychtoolbox.
%
% Be aware that execution of the PsychtoolboxVersion command can take a
% lot of time (in the order of multiple seconds to 1 minute).
%
% Most Psychtoolbox mex files now provide a built-in 'Version' command
% which returns version for themselves.  The version string for the
% built-in version numbers contains a fourth numeric field named "build".
% The build number is a unique serial number.  Mex files distinquished only
% by build numbers were compiled from identical C source files. 
%
% OS 9 : __________________________________________________________________
%
% versionNumber=PsychtoolboxVersion
%
% Return a number identifying this release of the Psychtoolbox.   Digits to
% the left of the decimal point are the major version number.  Digits to
% the right of the decimal point are the minor version number.  
% _________________________________________________________________________
%
% see also: Screen('Version')

%   2/9/01      awi     added fullfile command for platform-independent pathname   
%   6/29/02     dgp     Use new PsychtoolboxRoot function to cope with user-changed folder names.
%   7/12/04     awi     ****** OS X-specific fork from the OS 9 version *******
%                       Noted mex file versioning. 
%   7/26/04     awi     Partitioned help and added OS X section.  Enhanced
%                       OS9+Windows section.
%   10/4/05	   awi      Note here that dgp changed "IsWindows" to "IsWin" at unknown date prior
%						between 7/26/04 and 10/4/05.    
%
%   5/5/06     mk       Tries to query info from Subversion and displays info about last date
%                       of change, SVN revision and flavor. This code is pretty experimental and
%                       probably also somewhat fragile. And its sloooow.
%   9/17/06    mk       Improvements to parser: We try harder to get the flavor info.

global Psychtoolbox

if IsOS9
	if ~isfield(Psychtoolbox,'version')
		% Get version and date of Psychtoolbox from Psychtoolbox:Contents.m
		Psychtoolbox.version=0;
		file=fullfile(PsychtoolboxRoot,'Contents.m');
		f=fopen(file,'r');
		fgetl(f);
		s=fgetl(f);
		fclose(f);
		[Psychtoolbox.version,count,errmsg,n]=sscanf(s,'%% Version %f',1);
		ss=s(n:end);
		Psychtoolbox.date=ss(min(find(ss-' ')):end);
	end
	v=Psychtoolbox.version;
elseif IsOSX | IsLinux | IsWin
    if ~isfield(Psychtoolbox,'version')
        Psychtoolbox.version.major=0;
        Psychtoolbox.version.minor=0;
        Psychtoolbox.version.point=0;
        Psychtoolbox.version.string='';
        Psychtoolbox.version.flavor='';
        Psychtoolbox.version.revision=0;
        Psychtoolbox.version.revstring='';
        Psychtoolbox.version.websvn='';
        
        file=fullfile(PsychtoolboxRoot,'Contents.m');
		f=fopen(file,'r');
		fgetl(f);
		s=fgetl(f);
		fclose(f);
        [cvv,count,errmsg,n]=sscanf(s,'%% Version %d.%d.%d',3);
        Psychtoolbox.version.major=cvv(1);
        Psychtoolbox.version.minor=cvv(2);
        Psychtoolbox.version.point=cvv(3);

        % Additional parser code for SVN information. This is slooow!
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
            colpos=findstr(result, ':');
            if isempty(colpos)
                Psychtoolbox.version.revision=sscanf(result, '%d',1);
            else
                cvv = sscanf(result, '%d:%d',2);
                Psychtoolbox.version.revision=cvv(2);
            end
            if isempty(findstr(result, 'M'))
                Psychtoolbox.version.revstring = sprintf('Corresponds to SVN Revision %d', Psychtoolbox.version.revision);
            else
                Psychtoolbox.version.revstring = sprintf('Corresponds to SVN Revision %d but is *locally modified* !', Psychtoolbox.version.revision);
            end
            
            % Ok, now find the flavor and such... This is super expensive - needs network access...
            %[status , result] = system([svncmdpath 'svn info --xml -r ' num2str(Psychtoolbox.version.revision) '  ' PsychtoolboxRoot]);
            status=-1;
            if status<0 | status>0
                % Fallback path:
                if ~IsWin
                   [status , result] = system([svncmdpath 'svn info --xml ' PsychtoolboxRoot]);
                else
                   [status , result] = dos([svncmdpath 'svn info --xml ' PsychtoolboxRoot]);
                end
            end
            
            startdel = findstr(result, '/osxptb/') + length('/osxptb/');
            
            if isempty(startdel)
                if ~IsWin
                   [status , result] = system([svncmdpath 'svn info ' PsychtoolboxRoot]);
                else
                   [status , result] = dos([svncmdpath 'svn info ' PsychtoolboxRoot]);
                end
                startdel = findstr(result, '/osxptb/') + length('/osxptb/');
            end
            
            findel = min(findstr(result(startdel:length(result)), '/Psychtoolbox')) + startdel - 2;
            Psychtoolbox.version.flavor = result(startdel:findel);
            
            % And the date of last commit:
            startdel = findstr(result, '<date>') + length('<date>');
            findel = findstr(result, 'T') - 1;
            Psychtoolbox.date = result(startdel:findel);            
            % Build final SVN URL: This is the location where one can find detailled info about this working copy:
            Psychtoolbox.version.websvn = sprintf('http://svn.berlios.de/wsvn/osxptb/?rev=%d&sc=0', Psychtoolbox.version.revision);
            % Build final version string:
            Psychtoolbox.version.string = sprintf('%d.%d.%d - Flavor: %s - %s\nFor more info visit:\n%s', Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point, ...
                                                  Psychtoolbox.version.flavor, Psychtoolbox.version.revstring, Psychtoolbox.version.websvn);
        else        
            % Fallback path if svn commands fail for some reason. Output as much as we can.
            fprintf('PsychtoolboxVersion: WARNING - Could not query additional version information from SVN -- svn tools not properly installed?!?\n');
            Psychtoolbox.version.string=sprintf('%d.%d.%d', Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point);
            ss=s(n:end);
            Psychtoolbox.date=ss(min(find(ss-' ')):end);
        end    
    end
    versionString=Psychtoolbox.version.string;
    versionStructure=Psychtoolbox.version;
else
    error('Unrecognized Psychtoolbox platform');
end
