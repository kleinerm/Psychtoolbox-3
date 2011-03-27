function data = alGetFloatv( param )

% alGetFloatv  Interface to OpenAL function alGetFloatv
%
% usage:  data = alGetFloatv( param )
%
% C function:  void alGetFloatv(ALenum param, ALfloat* data)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

data = single(0);

moalcore( 'alGetFloatv', param, data );

return
