function retval = mogldouble(arg)
% --- OBSOLETE --- OBSOLETE --- OBSOLETE --- OBSOLETE ---
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
% as on Matlab and Octave 3.2+, it calls the builtin
% double() function.
%
% Otherwise (Octave) it would call our own special
% implementation.
%
% This is no longer needed as of Octave 3.2.0, but we leave the function
% here as many internal and external code relies on its presence.
%
% For Octave pre 3.2, this applied:
%
% This is a hack needed to make OpenGL (MOGL) work
% on GNU/Octave, despite Octave's lack of a single
% precision data type.
%

% ---protected---

if IsOctave
    % If this is Octave, then we can use dispatch() to optimize calls to
    % mogldouble() away, ie., all calls to mogldouble() will get redirected
    % to double() unconditionally to save some overhead:
    dispatch('mogldouble', 'double', 'all');
end

retval = double(arg);

return;

% % Special float values are encoded by MOGL/Octave
% % as uint32's. Therefore if the passed value isn't a
% % uint32, we apply the normal builtin double() operator:
% if ~strcmp(class(arg), 'uint32')
%    retval = double(arg);
% else
%    % This is a float, packed into a uint32. Apply our
%    % special cast-operator:
%    retval = castDouble2Float(arg, 1);
% end
%
%return;
