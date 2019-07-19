function [ value1, value2, value3 ] = alGetSource3f( sid, param )

% alGetSource3f  Interface to OpenAL function alGetSource3f
%
% usage:  [ value1, value2, value3 ] = alGetSource3f( sid, param )
%
% C function:  void alGetSource3f(ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2
    error('invalid number of arguments');
end

value1 = single([0,0]);
value2 = single([0,0]);
value3 = single([0,0]);
moalcore( 'alGetSource3f', sid, param, value1, value2, value3 );
value1 = value1(1);
value2 = value2(1);
value3 = value3(1);

return
