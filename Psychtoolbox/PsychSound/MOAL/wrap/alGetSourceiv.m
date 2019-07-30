function values = alGetSourceiv( sid, param )

% alGetSourceiv  Interface to OpenAL function alGetSourceiv
%
% usage:  values = alGetSourceiv( sid, param )
%
% C function:  void alGetSourceiv(ALuint sid, ALenum param, ALint* values)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=2
    error('invalid number of arguments');
end

values = int32(repmat(intmax,[ 32 1 ]));
moalcore( 'alGetSourceiv', sid, param, values );
values = values(find(values~=intmax));

return
