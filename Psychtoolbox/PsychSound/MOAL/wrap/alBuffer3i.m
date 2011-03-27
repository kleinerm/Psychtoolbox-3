function alBuffer3i( bid, param, value1, value2, value3 )

% alBuffer3i  Interface to OpenAL function alBuffer3i
%
% usage:  alBuffer3i( bid, param, value1, value2, value3 )
%
% C function:  void alBuffer3i(ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moalcore( 'alBuffer3i', bid, param, value1, value2, value3 );

return
