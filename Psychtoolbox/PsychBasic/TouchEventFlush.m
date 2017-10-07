function nflushed = TouchEventFlush(deviceIndex)
% nflushed = TouchEventFlush(deviceIndex)
%
% Flush event buffer of a touch queue. This removes all stored events
% from the event buffer of a given queue. It returns the number of
% removed events in the optional return argument 'nflushed'.
%

if nargin < 1 || isempty(deviceIndex)
  error('Required deviceIndex missing.');
end

nflushed = KbEventFlush(deviceIndex);
