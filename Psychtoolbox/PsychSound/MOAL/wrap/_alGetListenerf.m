function value = alGetListenerf( param )

% alGetListenerf  Interface to OpenAL function alGetListenerf
%
% usage:  value = alGetListenerf( param )
%
% C function:  void alGetListenerf(ALenum param, ALfloat* value)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

value = single(0);

moalcore( 'alGetListenerf', param, value );

return
