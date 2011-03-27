function values = alGetListeneriv( param )

% alGetListeneriv  Interface to OpenAL function alGetListeneriv
%
% usage:  values = alGetListeneriv( param )
%
% C function:  void alGetListeneriv(ALenum param, ALint* values)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

values = int32(0);

moalcore( 'alGetListeneriv', param, values );

return
