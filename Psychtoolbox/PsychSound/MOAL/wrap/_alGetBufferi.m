function value = alGetBufferi( bid, param )

% alGetBufferi  Interface to OpenAL function alGetBufferi
%
% usage:  value = alGetBufferi( bid, param )
%
% C function:  void alGetBufferi(ALuint bid, ALenum param, ALint* value)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value = int32(0);

moalcore( 'alGetBufferi', bid, param, value );

return
