function alSourceUnqueueBuffers( sid, numEntries, bids )

% alSourceUnqueueBuffers  Interface to OpenAL function alSourceUnqueueBuffers
%
% usage:  alSourceUnqueueBuffers( sid, numEntries, bids )
%
% C function:  void alSourceUnqueueBuffers(ALuint sid, ALsizei numEntries, ALuint* bids)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

bids = uint32(bids);

moalcore( 'alSourceUnqueueBuffers', sid, numEntries, bids );

return
