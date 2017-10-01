function navail = TouchEventAvail(deviceIndex)
% navail = TouchEventAvail(deviceIndex)
%
% Report number of events in a touch queue.
%

if nargin < 1 || isempty(deviceIndex)
  error('Required deviceIndex missing.');
end

navail = KbEventAvail(deviceIndex);
