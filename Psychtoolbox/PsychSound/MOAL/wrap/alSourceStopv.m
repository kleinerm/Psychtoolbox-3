function alSourceStopv( ns, sids )

% alSourceStopv  Interface to OpenAL function alSourceStopv
%
% usage:  alSourceStopv( ns, sids )
%
% C function:  void alSourceStopv(ALsizei ns, const ALuint* sids)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moalcore( 'alSourceStopv', ns, uint32(sids) );

return
