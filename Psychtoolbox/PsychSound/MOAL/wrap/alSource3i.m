function alSource3i( sid, param, value1, value2, value3 )

% alSource3i  Interface to OpenAL function alSource3i
%
% usage:  alSource3i( sid, param, value1, value2, value3 )
%
% C function:  void alSource3i(ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3)

% 27-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moalcore( 'alSource3i', sid, param, value1, value2, value3 );

return
