function TouchQueueStart(deviceIndex)
% TouchQueueStart(deviceIndex)
%
% Starts delivering touch events from the specified device to the 
% queue.
% _________________________________________________________________________
%

if nargin < 1 || isempty(deviceIndex)
  error('Required deviceIndex missing.');
end

KbQueueStart(deviceIndex);
