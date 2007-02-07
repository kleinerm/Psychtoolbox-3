function alSourceStop( sid )

% alSourceStop  Interface to OpenAL function alSourceStop
%
% usage:  alSourceStop( sid )
%
% C function:  void alSourceStop(ALuint sid)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alSourceStop', sid );

return
