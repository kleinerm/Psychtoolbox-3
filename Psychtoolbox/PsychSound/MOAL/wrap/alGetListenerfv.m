function values = alGetListenerfv( param )

% alGetListenerfv  Interface to OpenAL function alGetListenerfv
%
% usage:  values = alGetListenerfv( param )
%
% C function:  void alGetListenerfv(ALenum param, ALfloat* values)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=1,
    error('invalid number of arguments');
end

values = single(NaN(6,1));
moalcore( 'alGetListenerfv', param, values );
values = values(find(~isnan(values))); %#ok<FNDSB>

return
