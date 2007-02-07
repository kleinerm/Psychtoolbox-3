function alSourcef( sid, param, value )

% alSourcef  Interface to OpenAL function alSourcef
%
% usage:  alSourcef( sid, param, value )
%
% C function:  void alSourcef(ALuint sid, ALenum param, ALfloat value)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moalcore( 'alSourcef', sid, param, value );

return
