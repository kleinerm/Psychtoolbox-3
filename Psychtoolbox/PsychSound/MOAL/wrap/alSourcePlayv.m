function alSourcePlayv( ns, sids )

% alSourcePlayv  Interface to OpenAL function alSourcePlayv
%
% usage:  alSourcePlayv( ns, sids )
%
% C function:  void alSourcePlayv(ALsizei ns, const ALuint* sids)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alSourcePlayv', ns, uint32(sids) );

return
