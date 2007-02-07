function values = alGetSourcefv( sid, param )

% alGetSourcefv  Interface to OpenAL function alGetSourcefv
%
% usage:  values = alGetSourcefv( sid, param )
%
% C function:  void alGetSourcefv(ALuint sid, ALenum param, ALfloat* values)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

values = moglsingle(0);

moalcore( 'alGetSourcefv', sid, param, values );

return
