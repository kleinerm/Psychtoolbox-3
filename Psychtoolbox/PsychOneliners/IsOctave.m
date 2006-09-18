function resultFlag = IsOctave
% resultFlag = IsOctave
%
% Returns true if the script is running under GNU/Octave.
%
% Some scripts need to behave differently when running under
% GNU/Octave instead of running under Mathworks Matlab.

% History:
% 05/10/06 Written (MK).
% 03/08/06 Added the 'var' parameter to the exist function to make it
%          faster.  On my box it cuts 4+ ms off this function call. (CGB)
% 18/09/06 'var' parameter was wrong! It is a 'builtin' parameter (MK).

% If the built-in variable OCTAVE_VERSION exists,
% then we are running under GNU/Octave, otherwise not.
if exist('OCTAVE_VERSION', 'builtin') == 102
  resultFlag = 1;
else
  resultFlag = 0;
end;

return;
