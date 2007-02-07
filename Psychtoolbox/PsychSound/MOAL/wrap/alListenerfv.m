function alListenerfv( param, values )

% alListenerfv  Interface to OpenAL function alListenerfv
%
% usage:  alListenerfv( param, values )
%
% C function:  void alListenerfv(ALenum param, const ALfloat* values)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alListenerfv', param, moglsingle(values) );

return
