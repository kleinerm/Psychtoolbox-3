function alListener3f( param, value1, value2, value3 )

% alListener3f  Interface to OpenAL function alListener3f
%
% usage:  alListener3f( param, value1, value2, value3 )
%
% C function:  void alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moalcore( 'alListener3f', param, value1, value2, value3 );

return
