function [ value1, value2, value3 ] = alGetListener3f( param )

% alGetListener3f  Interface to OpenAL function alGetListener3f
%
% usage:  [ value1, value2, value3 ] = alGetListener3f( param )
%
% C function:  void alGetListener3f(ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

value1 = moglsingle(0);
value2 = moglsingle(0);
value3 = moglsingle(0);

moalcore( 'alGetListener3f', param, value1, value2, value3 );

return
