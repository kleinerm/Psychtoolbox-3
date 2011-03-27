function values = alGetSourcefv( sid, param )

% alGetSourcefv  Interface to OpenAL function alGetSourcefv
%
% usage:  values = alGetSourcefv( sid, param )
%
% C function:  void alGetSourcefv(ALuint sid, ALenum param, ALfloat* values)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

values = single(0);

moalcore( 'alGetSourcefv', sid, param, values );

return
