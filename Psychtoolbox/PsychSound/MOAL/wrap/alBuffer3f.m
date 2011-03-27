function alBuffer3f( bid, param, value1, value2, value3 )

% alBuffer3f  Interface to OpenAL function alBuffer3f
%
% usage:  alBuffer3f( bid, param, value1, value2, value3 )
%
% C function:  void alBuffer3f(ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moalcore( 'alBuffer3f', bid, param, value1, value2, value3 );

return
