function value = alGetListeneri( param )

% alGetListeneri  Interface to OpenAL function alGetListeneri
%
% usage:  value = alGetListeneri( param )
%
% C function:  void alGetListeneri(ALenum param, ALint* value)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

value = int32(0);

moalcore( 'alGetListeneri', param, value );

return
