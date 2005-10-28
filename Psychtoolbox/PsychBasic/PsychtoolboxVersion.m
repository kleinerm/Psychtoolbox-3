function [versionString, versionStructure]=PsychtoolboxVersion
% OS X: ___________________________________________________________________
% 
% [versionString, versionStructure]=PsychtoolboxVersion
%
% Return a string identifying this release of the Psychtoolbox.  The string
% consists of three integers separated by decimal points, for example:
% 1.0.1.  Each of the three integer fields has a particular meaning.  
%
% ¥ Leftmost: increments indicate a significant change in the feature
% set, either through accumulated progress over time or abrupt introduction
% of significant new features.
%
% ¥ Middle: Even numbers designate a "stable" release. The objective for
% even number releases is that the software should run stably, as opposed
% to introduction of new features. An even number is not a gurantee of
% stability, but an expression of intent.  Odd numbers indicate a
% "developer" release.  Odd number releases are incomplete, the software is
% made available for the purpose of public collaboration in development.
%
% ¥ Rightmost: A counter to distinguish multiple releases having the same
% leftmost and middle version numbers.
%
% Numeric values of the three integer fields contained in versionString are
% available in fields of the second return argument, "versionStructure".
%
% Most Psychtoolbox mex files now provide a built-in 'Version' command
% which returns version for themselves.  The version string for the
% built-in version numbers contains a fourth numeric field named "build".
% The build number is a unique serial number.  Mex files distinquished only
% by build numbers were compiled from identical C source files. 
%
% OS 9 and Windows: _______________________________________________________
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
%                        Noted mex file versioning. 
%   7/26/04     awi     Partitioned help and added OS X section.  Enhanced
%                       OS9+Windows section.
%   10/4/05	   awi   Note here that dgp changed "IsWindows" to "IsWin" at unknown date prior
%							between 7/26/04 and 10/4/05.    

global Psychtoolbox

if IsOS9 || IsWin
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
elseif IsOSX
    if ~isfield(Psychtoolbox,'version')
        Psychtoolbox.version.major=0;
        Psychtoolbox.version.minor=0;
        Psychtoolbox.version.point=0;
        Psychtoolbox.version.string='';
 		file=fullfile(PsychtoolboxRoot,'Contents.m');
		f=fopen(file,'r');
		fgetl(f);
		s=fgetl(f);
		fclose(f);
        [cvv,count,errmsg,n]=sscanf(s,'%% Version %d.%d.%d',3);
        Psychtoolbox.version.major=cvv(1);
        Psychtoolbox.version.minor=cvv(2);
        Psychtoolbox.version.point=cvv(3);
        Psychtoolbox.version.string=sprintf('%d.%d.%d', Psychtoolbox.version.major, Psychtoolbox.version.minor, Psychtoolbox.version.point);
        ss=s(n:end);
		Psychtoolbox.date=ss(min(find(ss-' ')):end);
    end
    versionString=Psychtoolbox.version.string;
    versionStructure=Psychtoolbox.version;
else
    error('Unrecognized Psychtoolbox platform');
end
   


