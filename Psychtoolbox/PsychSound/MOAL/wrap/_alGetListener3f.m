function [ value1, value2, value3 ] = alGetListener3f( param )

% alGetListener3f  Interface to OpenAL function alGetListener3f
%
% usage:  [ value1, value2, value3 ] = alGetListener3f( param )
%
% C function:  void alGetListener3f(ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

value1 = single(0);
value2 = single(0);
value3 = single(0);

moalcore( 'alGetListener3f', param, value1, value2, value3 );

return
