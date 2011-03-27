function alSourcefv( sid, param, values )

% alSourcefv  Interface to OpenAL function alSourcefv
%
% usage:  alSourcefv( sid, param, values )
%
% C function:  void alSourcefv(ALuint sid, ALenum param, const ALfloat* values)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moalcore( 'alSourcefv', sid, param, single(values) );

return
