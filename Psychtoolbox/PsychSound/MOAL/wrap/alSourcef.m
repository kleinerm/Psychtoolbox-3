function alSourcef( sid, param, value )

% alSourcef  Interface to OpenAL function alSourcef
%
% usage:  alSourcef( sid, param, value )
%
% C function:  void alSourcef(ALuint sid, ALenum param, ALfloat value)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moalcore( 'alSourcef', sid, param, value );

return
