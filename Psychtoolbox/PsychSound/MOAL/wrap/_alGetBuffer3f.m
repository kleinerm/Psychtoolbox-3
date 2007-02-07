function [ value1, value2, value3 ] = alGetBuffer3f( bid, param )

% alGetBuffer3f  Interface to OpenAL function alGetBuffer3f
%
% usage:  [ value1, value2, value3 ] = alGetBuffer3f( bid, param )
%
% C function:  void alGetBuffer3f(ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value1 = moglsingle(0);
value2 = moglsingle(0);
value3 = moglsingle(0);

moalcore( 'alGetBuffer3f', bid, param, value1, value2, value3 );

return
