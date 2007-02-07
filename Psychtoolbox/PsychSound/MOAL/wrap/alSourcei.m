function alSourcei( sid, param, value )

% alSourcei  Interface to OpenAL function alSourcei
%
% usage:  alSourcei( sid, param, value )
%
% C function:  void alSourcei(ALuint sid, ALenum param, ALint value)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moalcore( 'alSourcei', sid, param, value );

return
