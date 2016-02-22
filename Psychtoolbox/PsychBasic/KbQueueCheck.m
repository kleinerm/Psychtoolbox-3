function [pressed, firstPress, firstRelease, lastPress, lastRelease] = KbQueueCheck(deviceIndex)
%  [pressed, firstPress, firstRelease, lastPress, lastRelease] = KbQueueCheck([deviceIndex])
%
%  Obtains data about keypresses on the specified device since the 
%  most recent call to this routine, KbQueueStart, or KbQueueWait
%  Clears all scored events, but unscored events that are still being
%  processsed may remain in the queue
%
%  pressed: a boolean indicating whether a key has been pressed
%
%  firstPress: an array indicating the time that each key was first
%  pressed since the most recent call to KbQueueCheck or KbQueueStart
%
%  firstRelease: an array indicating the time that each key was first
%  released since the most recent call to KbQueueCheck or KbQueueStart
%
%  lastPress: an array indicating the most recent time that each key was
%  pressed since the most recent call to KbQueueCheck or KbQueueStart
%
%  lastRelease: an array indicating the most recent time that each key
%  was released since the most recent call to KbQueueCheck or 
%  KbQueueStart
%
%  For firstPress, firstRelease, lastPress and lastRelease, a time value
%  of zero indicates that no event for the corresponding key was
%  detected since the most recent call to KbQueueCheck or KbQueueStart
%
%  To identify specific keys, use KbName (e.g., KbName(firstPress)) to
%  generate a list of the keys for which the events occurred
%
%  For compatibility with KbCheck, any key codes stored in
%  ptb_kbcheck_disabledKeys (see "help DisableKeysForKbCheck"), will
%  not caused pressed to return as true and will be zeroed out in the
%  returned arrays. However, a better alternative is to specify a
%  keyList arguement to KbQueueCreate. 
%
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
if ~KbQueueReserve(3, 2, deviceIndex)
    if isempty(deviceIndex)
        deviceIndex = NaN;
    end
    error('Keyboard queue for device %i already in use by GetChar() et al. Use of GetChar and keyboard queues is mutually exclusive!', deviceIndex);
end

global ptb_kbcheck_disabledKeys;
if nargin==0
  [pressed, firstPress, firstRelease, lastPress, lastRelease] = PsychHID('KbQueueCheck');
elseif nargin > 0
  [pressed, firstPress, firstRelease, lastPress, lastRelease] = PsychHID('KbQueueCheck', deviceIndex);
end

% Any dead keys defined?
if ~isempty(ptb_kbcheck_disabledKeys)
   % Yes. Disable all dead keys - force them to 'not ever pressed or released':
   firstPress(ptb_kbcheck_disabledKeys)=0;
   firstRelease(ptb_kbcheck_disabledKeys)=0;
   lastPress(ptb_kbcheck_disabledKeys)=0;
   lastRelease(ptb_kbcheck_disabledKeys)=0;

   % Reevaluate global key down state:
   pressed = any(firstPress);
end
