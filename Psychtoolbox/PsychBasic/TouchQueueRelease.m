function TouchQueueRelease(deviceIndex)
% TouchQueueRelease(deviceIndex)
%
% Releases touch queue for the specified device.
% _________________________________________________________________________
%

if nargin < 1 || isempty(deviceIndex)
  error('Required deviceIndex missing.');
end

KbQueueRelease(deviceIndex);
