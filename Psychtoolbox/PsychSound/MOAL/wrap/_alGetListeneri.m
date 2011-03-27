function value = alGetListeneri( param )

% alGetListeneri  Interface to OpenAL function alGetListeneri
%
% usage:  value = alGetListeneri( param )
%
% C function:  void alGetListeneri(ALenum param, ALint* value)

% 27-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

value = int32(0);

moalcore( 'alGetListeneri', param, value );

return
