function resultFlag = IsWinMatlabR11Style
% resultFlag = IsWinMatlabR11Style
%
% Return true if Psychtoolbox is running on Microsoft Windows,
% with a Matlab version older than R2007a, i.e. using MEX files that
% are compiled against Matlab R11.
% 
% See also: IsOSX, IsLinux, IsOS9, IsWin, OSName, computer

% HISTORY
% 01-Mar-2010  mk    Wrote it.
% 16-Dec-2023  mk    This is never R11, as we do not support it since ages.
resultFlag = 0;
