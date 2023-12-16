function resultFlag = IsOSX(is64)

% resultFlag = IsOSX([is64=0])
%
% Returns true if the operating system is macOS.
% If the optional 'is64' flag is set to one, returns
% true if the runtime is 64 bit and on macOS.
%
% See also: IsWin, IsLinux, OSName, computer

% HISTORY
% ??/??/??  awi Wrote it.
% 06/30/06  awi Fixed help section.
% 04/30/12  mk  Support 64-Bit OS/X.
% 12/16/23  mk  Simplify.

persistent rc;
persistent rc64;

% check input
if nargin < 1 || isempty(is64)
     is64 = 0;
end

if isempty(rc)
     rc = ismac;
end

if isempty(rc64)
     rc64 = rc && Is64Bit;
end

if is64 == 0
     resultFlag = rc;
else
     resultFlag = rc64;
end
