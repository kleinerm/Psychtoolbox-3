function [ value1, value2, value3 ] = alGetBuffer3i( bid, param )

% alGetBuffer3i  Interface to OpenAL function alGetBuffer3i
%
% usage:  [ value1, value2, value3 ] = alGetBuffer3i( bid, param )
%
% C function:  void alGetBuffer3i(ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value1 = int32(0);
value2 = int32(0);
value3 = int32(0);

moalcore( 'alGetBuffer3i', bid, param, value1, value2, value3 );

return
