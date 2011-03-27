function [ value1, value2, value3 ] = alGetListener3i( param )

% alGetListener3i  Interface to OpenAL function alGetListener3i
%
% usage:  [ value1, value2, value3 ] = alGetListener3i( param )
%
% C function:  void alGetListener3i(ALenum param, ALint* value1, ALint* value2, ALint* value3)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

value1 = int32(0);
value2 = int32(0);
value3 = int32(0);

moalcore( 'alGetListener3i', param, value1, value2, value3 );

return
