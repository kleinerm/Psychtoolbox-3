function r = alGetError

% alGetError  Interface to OpenAL function alGetError
%
% usage:  r = alGetError
%
% C function:  ALenum alGetError(void)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

r = moalcore( 'alGetError' );

return
