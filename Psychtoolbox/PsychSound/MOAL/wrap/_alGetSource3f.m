function [ value1, value2, value3 ] = alGetSource3f( sid, param )

% alGetSource3f  Interface to OpenAL function alGetSource3f
%
% usage:  [ value1, value2, value3 ] = alGetSource3f( sid, param )
%
% C function:  void alGetSource3f(ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value1 = moglsingle(0);
value2 = moglsingle(0);
value3 = moglsingle(0);

moalcore( 'alGetSource3f', sid, param, value1, value2, value3 );

return
