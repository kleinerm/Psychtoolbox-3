function values = alGetBufferfv( bid, param )

% alGetBufferfv  Interface to OpenAL function alGetBufferfv
%
% usage:  values = alGetBufferfv( bid, param )
%
% C function:  void alGetBufferfv(ALuint bid, ALenum param, ALfloat* values)

% 28-Mar-2011 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=2,
    error('invalid number of arguments');
end

values = single(NaN(6,1));
moalcore( 'alGetBufferfv', bid, param, values );
values = values(find(~isnan(values))); %#ok<FNDSB>

return
