function alSourceQueueBuffers( sid, numEntries, bids )

% alSourceQueueBuffers  Interface to OpenAL function alSourceQueueBuffers
%
% usage:  alSourceQueueBuffers( sid, numEntries, bids )
%
% C function:  void alSourceQueueBuffers(ALuint sid, ALsizei numEntries, const ALuint* bids)

% 28-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moalcore( 'alSourceQueueBuffers', sid, numEntries, uint32(bids) );

return
