function values = alGetListenerfv( param )

% alGetListenerfv  Interface to OpenAL function alGetListenerfv
%
% usage:  values = alGetListenerfv( param )
%
% C function:  void alGetListenerfv(ALenum param, ALfloat* values)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

values = single(0);

moalcore( 'alGetListenerfv', param, values );

return
