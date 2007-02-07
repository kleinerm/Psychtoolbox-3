function value = alGetSourcei( sid, param )

% alGetSourcei  Interface to OpenAL function alGetSourcei
%
% usage:  value = alGetSourcei( sid, param )
%
% C function:  void alGetSourcei(ALuint sid, ALenum param, ALint* value)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

value = int32(0);

moalcore( 'alGetSourcei', sid, param, value );

return
