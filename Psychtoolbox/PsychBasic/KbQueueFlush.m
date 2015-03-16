function nflushed = KbQueueFlush(deviceIndex, flushType)
%  nflushed = KbQueueFlush([deviceIndex][flushType=1])
%
%  Flush KbQueue and/or KbQueue event buffer. By default, if flushType is
%  omitted, only the KbQueues events are deleted. Other 'flushTypes' affect
%  the KbQueue event buffer, but rather use the KbBufferFlush() function to
%  do this.
%
%  If 'flushType' is 0, only the number of currently queued events will be
%  returned.
%
%  If 'flushType' is 1, only events returned by KbQueueCheck will be flushed. This
%  is the default.
%
%  If 'flushType' is 2, only events returned by KbQueueGetEvent will be flushed.
%
%  If 'flushType' is 3, events returned by both KbQueueCheck and KbQueueGetEvent
%  will be flushed.
%
%  If 'flushType' is 4, only the number of key-press events with valid, mapped ASCII
%  CookedKey field will be returned.
%
%  Removes all unprocessed events from the queue and zeros out any already
%  scored events.
% _________________________________________________________________________
%
% See also: KbQueueCreate, KbQueueStart, KbQueueStop, KbQueueCheck,
%            KbQueueWait, KbQueueFlush, KbQueueRelease

% 8/23/07    rpw  Wrote it.

if nargin < 1
    deviceIndex = [];
end

% Try to check if keyboard queue for 'deviceIndex' is reserved for our exclusive use:
if ~KbQueueReserve(3, 2, deviceIndex) && KbQueueReserve(3, 1, deviceIndex)
    error('Keyboard queue for device %i already in use by GetChar() et al. Use of GetChar and keyboard queues is mutually exclusive!', deviceIndex);
end

if nargin == 0
    nflushed = PsychHID('KbQueueFlush');
elseif nargin > 0
    if nargin < 2 || isempty(flushType)
        flushType = [];
    end
    nflushed = PsychHID('KbQueueFlush', deviceIndex, flushType);
end
