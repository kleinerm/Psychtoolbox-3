function resultFlag = IsLinux

% resultFlag = IsLinux
%
% Returns true if the operating system is Linux.
% 
% See also: IsOSX, IsWin, IsOS9, OSName, computer

% HISTORY
% ??/??/?? ??? Wrote it.
% 6/30/06  awi Fixed help section.  


resultFlag= streq(computer,'GLNX86') | ~isempty(findstr(computer, 'linux-gnu'));
