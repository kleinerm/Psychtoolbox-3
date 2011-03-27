function values = alGetSourceiv( sid, param )

% alGetSourceiv  Interface to OpenAL function alGetSourceiv
%
% usage:  values = alGetSourceiv( sid, param )
%
% C function:  void alGetSourceiv(ALuint sid, ALenum param, ALint* values)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

values = int32(0);

moalcore( 'alGetSourceiv', sid, param, values );

return
