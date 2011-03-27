function r = alGetError

% alGetError  Interface to OpenAL function alGetError
%
% usage:  r = alGetError
%
% C function:  ALenum alGetError(void)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

r = moalcore( 'alGetError' );

return
