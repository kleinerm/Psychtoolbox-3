function value = alGetBufferf( bid, param )

% alGetBufferf  Interface to OpenAL function alGetBufferf
%
% usage:  value = alGetBufferf( bid, param )
%
% C function:  void alGetBufferf(ALuint bid, ALenum param, ALfloat* value)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value = single(0);

moalcore( 'alGetBufferf', bid, param, value );

return
