function alSourcePlay( sid )

% alSourcePlay  Interface to OpenAL function alSourcePlay
%
% usage:  alSourcePlay( sid )
%
% C function:  void alSourcePlay(ALuint sid)

% 06-Feb-2007 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moalcore( 'alSourcePlay', sid );

return
