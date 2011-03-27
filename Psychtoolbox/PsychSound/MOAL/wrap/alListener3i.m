function alListener3i( param, value1, value2, value3 )

% alListener3i  Interface to OpenAL function alListener3i
%
% usage:  alListener3i( param, value1, value2, value3 )
%
% C function:  void alListener3i(ALenum param, ALint value1, ALint value2, ALint value3)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moalcore( 'alListener3i', param, value1, value2, value3 );

return
