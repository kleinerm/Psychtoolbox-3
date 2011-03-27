function alSource3f( sid, param, value1, value2, value3 )

% alSource3f  Interface to OpenAL function alSource3f
%
% usage:  alSource3f( sid, param, value1, value2, value3 )
%
% C function:  void alSource3f(ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moalcore( 'alSource3f', sid, param, value1, value2, value3 );

return
