function alListeneri( param, value )

% alListeneri  Interface to OpenAL function alListeneri
%
% usage:  alListeneri( param, value )
%
% C function:  void alListeneri(ALenum param, ALint value)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alListeneri', param, value );

return
