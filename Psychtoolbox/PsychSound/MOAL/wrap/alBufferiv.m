function alBufferiv( bid, param, values )

% alBufferiv  Interface to OpenAL function alBufferiv
%
% usage:  alBufferiv( bid, param, values )
%
% C function:  void alBufferiv(ALuint bid, ALenum param, const ALint* values)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moalcore( 'alBufferiv', bid, param, int32(values) );

return
