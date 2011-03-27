function alSourcePause( sid )

% alSourcePause  Interface to OpenAL function alSourcePause
%
% usage:  alSourcePause( sid )
%
% C function:  void alSourcePause(ALuint sid)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alSourcePause', sid );

return
