function alListeneriv( param, values )

% alListeneriv  Interface to OpenAL function alListeneriv
%
% usage:  alListeneriv( param, values )
%
% C function:  void alListeneriv(ALenum param, const ALint* values)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alListeneriv', param, int32(values) );

return
