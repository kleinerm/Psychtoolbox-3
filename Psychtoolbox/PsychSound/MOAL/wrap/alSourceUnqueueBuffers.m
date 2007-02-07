function bids = alSourceUnqueueBuffers( sid, numEntries )

% alSourceUnqueueBuffers  Interface to OpenAL function alSourceUnqueueBuffers
%
% usage:  bids = alSourceUnqueueBuffers( sid, numEntries )
%
% C function:  void alSourceUnqueueBuffers(ALuint sid, ALsizei numEntries, ALuint* bids)

% 06-Feb-2007 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

bids = uint32(zeros(numEntries, 1));

moalcore( 'alSourceUnqueueBuffers', sid, numEntries, bids );

return
