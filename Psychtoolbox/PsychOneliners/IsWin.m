function resultFlag = IsWin

% resultFlag = IsWin
%
% Return true if the operating system is Windows.
% 
% See also: IsOSX, IsLinux, IsOS9, OSName, computer

% HISTORY
% ??/??/?? awi Wrote it.
% 6/30/06  awi Fixed help section.  

persistent rc;

if isempty(rc)
     rc=streq(computer,'PCWIN') | streq(computer,'PCWIN64');
end;
resultFlag=rc;
