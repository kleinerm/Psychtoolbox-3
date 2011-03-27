function alSourceRewind( sid )

% alSourceRewind  Interface to OpenAL function alSourceRewind
%
% usage:  alSourceRewind( sid )
%
% C function:  void alSourceRewind(ALuint sid)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alSourceRewind', sid );

return
