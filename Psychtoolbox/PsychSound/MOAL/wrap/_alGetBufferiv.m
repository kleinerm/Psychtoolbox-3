function values = alGetBufferiv( bid, param )

% alGetBufferiv  Interface to OpenAL function alGetBufferiv
%
% usage:  values = alGetBufferiv( bid, param )
%
% C function:  void alGetBufferiv(ALuint bid, ALenum param, ALint* values)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

values = int32(0);

moalcore( 'alGetBufferiv', bid, param, values );

return
