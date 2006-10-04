function resultFlag = IsOSX

% resultFlag = IsOSX
%
% Returns true if the operating system is Mac OS X.
% 
% See also: IsWin, IsLinux, IsOS9, OSName, computer

% HISTORY
% ??/??/?? awi Wrote it.
% 6/30/06  awi Fixed help section.  

persistent rc;

if isempty(rc)
     rc = streq(computer,'MAC') | streq(computer, 'MACI') | ~isempty(findstr(computer, 'apple-darwin'));
end;

resultFlag=rc;
