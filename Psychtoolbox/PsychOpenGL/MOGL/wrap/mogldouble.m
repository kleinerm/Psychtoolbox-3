function retval = mogldouble(arg)
%
% PSYCHTOOLBOX SPECIFIC double() implementation:
%
% retval = mogldouble(arg) -- convert into
% a double precision floating point number.
%
% This routine takes an 'arg' of arbitrary
% numeric class and converts it into an equivalent
% object of double precision floating point format.
%
% If a builtin double() function is available,
% as on Matlab, it calls the builtin double() function.
%
% Otherwise (Octave) it calls our own special
% implementation.
%
% This is a hack needed to make OpenGL (MOGL) work
% on GNU/Octave, despite Octave's lack of a single
% precision data type.
%

% Special float values are encoded by MOGL/Octave
% as uint32's. Therefore if the passed value isn't a
% uint32, we apply the normal builtin double() operator:
if ~strcmp(class(arg), 'uint32')
   retval = double(arg);
else
   % This is a float, packed into a uint32. Apply our
   % special cast-operator:
   retval = castDouble2Float(arg, 1);
end

return;
