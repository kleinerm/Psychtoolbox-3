function retval = moglsingle(arg)
%
% PSYCHTOOLBOX SPECIFIC single() implementation:
%
% retval = moglsingle(arg) -- convert into
% a single precision floating point number.
%
% This routine takes an 'arg' of arbitrary
% numeric class and converts it into an equivalent
% object of single precision floating point format.
%
% If a builtin single() function is available,
% as on Matlab, it calls the builtin single() function.
%
% Otherwise (Octave) it calls our own special
% implementation.
%
% This is a hack needed to make OpenGL (MOGL) work
% on GNU/Octave, despite Octave's lack of a single
% precision data type. Returned values are not
% useful or accessible by Octave. They only have
% meaning for the Psychtoolbox OpenGL functions.
%

if exist('single', 'builtin')==5
   retval = builtin('single', arg);
else
   retval = castDouble2Float(double(arg));
end

return;
