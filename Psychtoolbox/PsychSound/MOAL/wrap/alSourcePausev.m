function alSourcePausev( ns, sids )

% alSourcePausev  Interface to OpenAL function alSourcePausev
%
% usage:  alSourcePausev( ns, sids )
%
% C function:  void alSourcePausev(ALsizei ns, const ALuint* sids)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alSourcePausev', ns, uint32(sids) );

return
