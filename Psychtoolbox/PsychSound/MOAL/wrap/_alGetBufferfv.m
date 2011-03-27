function values = alGetBufferfv( bid, param )

% alGetBufferfv  Interface to OpenAL function alGetBufferfv
%
% usage:  values = alGetBufferfv( bid, param )
%
% C function:  void alGetBufferfv(ALuint bid, ALenum param, ALfloat* values)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

values = single(0);

moalcore( 'alGetBufferfv', bid, param, values );

return
