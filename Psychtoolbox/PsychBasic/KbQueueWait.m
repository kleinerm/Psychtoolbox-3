function secs=KbQueueWait(deviceIndex)
%  secs=KbQueueWait([deviceIndex])
%
%  Waits for any key to be pressed and returns the time of the press.
%
%  KbQueueFlush should be called immediately prior to this function
%  (unless the queue has just been created and started) to clear any 
%  prior events.
%
%  Note that this command will not respond to any keys that were 
%  inactivated by using the keyList argument to KbQueueCreate.
%
%  Since KbQueueWait is implemented as a looping call to
%  KbQueueCheck, it will not respond to any key codes stored in
%  the global variable ptb_kbcheck_disabledKeys
%  (see "help DisableKeysForKbCheck")
% _________________________________________________________________________
%
% See also: KbQueueCreate, KbQueueStart, KbQueueStop, KbQueueCheck,
%            KbQueueWait, KbQueueFlush, KbQueueRelease

% 8/19/07    rpw  Wrote it.
% 8/23/07    rpw  Modifications to add KbQueueFlush
% 5/14/12    mk   Small fixes: Use 1 msec wait interval.

if nargin < 1
  deviceIndex = [];
end

% Try to check if keyboard queue for 'deviceIndex' is reserved for our exclusive use:
if ~KbQueueReserve(3, 2, deviceIndex) && KbQueueReserve(3, 1, deviceIndex)
  error('Keyboard queue for device %i already in use by GetChar() et al. Use of GetChar and keyboard queues is mutually exclusive!', deviceIndex);
end

% It is implicit in invoking this function that the queue should be running
% and it is potentially problematic if it is not since the function will
% never return, therefore, go ahead and start the queue if it isn't running
KbQueueStart(deviceIndex);
while(1)
  [pressed, firstPress] = KbQueueCheck(deviceIndex);
  if pressed
    break;
  end

  % Wait for 1 msec to prevent system overload:
  WaitSecs('Yieldsecs', 0.001);
end

presses=find(firstPress);
secs=min(firstPress(presses)); %#ok<FNDSB>

return;
