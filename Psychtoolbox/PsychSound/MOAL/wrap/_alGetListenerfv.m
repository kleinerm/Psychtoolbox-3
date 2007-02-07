function values = alGetListenerfv( param )

% alGetListenerfv  Interface to OpenAL function alGetListenerfv
%
% usage:  values = alGetListenerfv( param )
%
% C function:  void alGetListenerfv(ALenum param, ALfloat* values)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

values = moglsingle(0);

moalcore( 'alGetListenerfv', param, values );

return
