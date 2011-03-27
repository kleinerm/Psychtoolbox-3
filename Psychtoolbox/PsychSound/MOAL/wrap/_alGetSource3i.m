function [ value1, value2, value3 ] = alGetSource3i( sid, param )

% alGetSource3i  Interface to OpenAL function alGetSource3i
%
% usage:  [ value1, value2, value3 ] = alGetSource3i( sid, param )
%
% C function:  void alGetSource3i(ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value1 = int32(0);
value2 = int32(0);
value3 = int32(0);

moalcore( 'alGetSource3i', sid, param, value1, value2, value3 );

return
