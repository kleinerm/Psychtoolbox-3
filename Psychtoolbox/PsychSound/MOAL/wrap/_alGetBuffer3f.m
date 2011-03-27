function [ value1, value2, value3 ] = alGetBuffer3f( bid, param )

% alGetBuffer3f  Interface to OpenAL function alGetBuffer3f
%
% usage:  [ value1, value2, value3 ] = alGetBuffer3f( bid, param )
%
% C function:  void alGetBuffer3f(ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value1 = single(0);
value2 = single(0);
value3 = single(0);

moalcore( 'alGetBuffer3f', bid, param, value1, value2, value3 );

return
