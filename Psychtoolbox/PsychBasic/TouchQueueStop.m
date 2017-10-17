function TouchQueueStop(deviceIndex)
% TouchQueueStop(deviceIndex)
%
% Stops delivering touch events from the specified device to the 
% queue.
% _________________________________________________________________________
%

if nargin < 1 || isempty(deviceIndex)
  error('Required deviceIndex missing.');
end

KbQueueStop(deviceIndex);
