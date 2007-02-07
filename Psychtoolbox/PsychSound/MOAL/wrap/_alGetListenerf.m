function value = alGetListenerf( param )

% alGetListenerf  Interface to OpenAL function alGetListenerf
%
% usage:  value = alGetListenerf( param )
%
% C function:  void alGetListenerf(ALenum param, ALfloat* value)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

value = moglsingle(0);

moalcore( 'alGetListenerf', param, value );

return
