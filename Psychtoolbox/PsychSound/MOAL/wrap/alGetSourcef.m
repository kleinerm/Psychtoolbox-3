function value = alGetSourcef( sid, param )

% alGetSourcef  Interface to OpenAL function alGetSourcef
%
% usage:  value = alGetSourcef( sid, param )
%
% C function:  void alGetSourcef(ALuint sid, ALenum param, ALfloat* value)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

value = single(0);

moalcore( 'alGetSourcef', sid, param, value );

return
