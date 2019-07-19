function values = alGetListeneriv( param )

% alGetListeneriv  Interface to OpenAL function alGetListeneriv
%
% usage:  values = alGetListeneriv( param )
%
% C function:  void alGetListeneriv(ALenum param, ALint* values)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=1
    error('invalid number of arguments');
end

values = int32(repmat(intmax,[ 32 1 ]));
moalcore( 'alGetListeneriv', param, values );
values = values(find(values~=intmax));

return
