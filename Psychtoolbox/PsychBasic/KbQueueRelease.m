function KbQueueRelease(deviceIndex)
% KbQueueRelease([deviceIndex])
%
% Releases queue-associated resources; once called, KbQueueCreate
% must be invoked before using any of the other routines
%
% This routine is called automatically at clean-up (e.g., when 
% 'clear mex' is invoked and can be omitted expense of keeping 
% memory allocated and an additional thread running unnecesarily
%_________________________________________________________________________
%
% See also: KbQueueCreate, KbQueueStart, KbQueueStop, KbQueueCheck,
%            KbQueueWait, KbQueueFlush, KbQueueRelease

% 8/19/07    rpw  Wrote it.
% 8/23/07    rpw  Modifications to add KbQueueFlush

if nargin < 1
    deviceIndex = [];
end

% Try to release keyboard queue for 'deviceIndex' from our exclusive use:
if ~KbQueueReserve(3, 2, deviceIndex)
  return;
end

KbQueueReserve(2, 2, deviceIndex);

if nargin == 0
  PsychHID('KbQueueRelease');
elseif nargin > 0
  PsychHID('KbQueueRelease', deviceIndex);
end
