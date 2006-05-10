function resultFlag = IsOctave
% resultFlag = IsOctave
%
% Returns true if the script is running under GNU/Octave.
%
% Some scripts need to behave differently when running under
% GNU/Octave instead of running under Mathworks Matlab.

% History:
% 05/10/06 Written (MK).

% If the built-in variable OCTAVE_VERSION exists,
% then we are running under GNU/Octave, otherwise not.
if exist('OCTAVE_VERSION') == 102
  resultFlag = 1;
else
  resultFlag = 0;
end;

return;
