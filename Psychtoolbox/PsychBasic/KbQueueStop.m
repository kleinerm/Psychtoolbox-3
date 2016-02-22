function KbQueueStop(deviceIndex)
% KbQueueStop([deviceIndex])
%
% Stops delivery of new keyboard events from the specified device to 
% the queue.
%
% Data regarding events already queued is not cleared and can be 
% recovered by KbQueueCheck
% _________________________________________________________________________
%
% See also: KbQueueCreate, KbQueueStart, KbQueueStop, KbQueueCheck,
%            KbQueueWait, KbQueueFlush, KbQueueRelease

% 8/19/07    rpw  Wrote it.
% 8/23/07    rpw  Modifications to add KbQueueFlush

if nargin < 1
    deviceIndex = [];
end

% Try to check if keyboard queue for 'deviceIndex' is reserved for our exclusive use:
if ~KbQueueReserve(3, 2, deviceIndex) && KbQueueReserve(3, 1, deviceIndex)
    if isempty(deviceIndex)
        deviceIndex = NaN;
    end
    error('Keyboard queue for device %i already in use by GetChar() et al. Use of GetChar and keyboard queues is mutually exclusive!', deviceIndex);
end

if nargin == 0
  PsychHID('KbQueueStop');
elseif nargin > 0
  PsychHID('KbQueueStop', deviceIndex);
end
