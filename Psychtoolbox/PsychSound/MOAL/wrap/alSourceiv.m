function alSourceiv( sid, param, values )

% alSourceiv  Interface to OpenAL function alSourceiv
%
% usage:  alSourceiv( sid, param, values )
%
% C function:  void alSourceiv(ALuint sid, ALenum param, const ALint* values)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moalcore( 'alSourceiv', sid, param, int32(values) );

return
