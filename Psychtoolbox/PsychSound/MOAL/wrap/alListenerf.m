function alListenerf( param, value )

% alListenerf  Interface to OpenAL function alListenerf
%
% usage:  alListenerf( param, value )
%
% C function:  void alListenerf(ALenum param, ALfloat value)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alListenerf', param, value );

return
