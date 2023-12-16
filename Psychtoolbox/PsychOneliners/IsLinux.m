function resultFlag = IsLinux(is64)

% resultFlag = IsLinux([is64=0])
%
% Returns true if the operating system is Linux.
% If optional 'is64' flag is set to one, returns
% true if the runtime is 64 bit and on Linux.
% 
% See also: IsOSX, IsWin, OSName, computer

% HISTORY
% ??/??/?? ??? Wrote it.
% 6/30/06  awi Fixed help section.  
% 6/18/11   mk Handle query if 64 bit runtime as well.
% 6/13/12   dn Simplified
% 12/16/23  mk Simplify.

persistent rc;
persistent rc64;

% check input
if nargin < 1 || isempty(is64)
     is64 = 0;
end

if isempty(rc)
     rc= isunix && ~ismac;
end

if isempty(rc64)
     rc64 = rc && Is64Bit;
end

if is64 == 0
     resultFlag = rc;
else
     resultFlag = rc64;
end
