function secs=KbQueueWait(deviceIndex, forWhat, untilTime)
% secs=KbQueueWait([deviceIndex][, forWhat=0][, untilTime=inf])
%
% Waits for any key to be pressed and returns the time of the press. The
% optional 'forWhat' parameter controls what kind of event is waited for
% (similar to KbWait).
% forWhat:
% 0: return as soon as a key is down
% 1: return as soon as no keys are down
% 2: wait for a key press (so if a key is already down upon function
%    entering, it is ignored):
%    waitForAllKeysReleased -> waitForKeypress
% 3: wait for a keystroke like, 2, but wait until key is released after
%    pressing it:
%    waitForAllKeysReleased -> waitForKeypress -> waitForAllKeysReleased
%
% If the optional parameter 'untilTime' is provided, KbWait will only wait
% until that time and then return regardless if anything happened on the
% keyboard or not.
%
% KbQueueFlush should not be called immediately prior to this function. If
% you do that and clear prior events, KbQueueWait is not able to determine
% whether a key is down or not upon function entry (important for all four
% forWhat modes). It will assume no keys are down and, e.g., return
% immediate when forWhat is 1 even though a key was depressed before the
% flush and is still being held. KbQueueWait correctly deals with previous
% events in the buffer.
% NB. Previously, use of KbQueueFlush before calling this function was
% recommended. The current function maintains backwards compatibility for
% this use case. However, any changes of existing code to follow the new
% recommendation and remove preceding KbQueueFlush calls should be
% carefully considered and tested (don't change what works).
%
% Note that this command will not respond to any keys that were inactivated
% by using the keyList argument to KbQueueCreate.
%
% Since KbQueueWait is implemented as a looping call to KbQueueCheck, it
% will not respond to any key codes stored in the global variable
% ptb_kbcheck_disabledKeys (see "help DisableKeysForKbCheck")
% _________________________________________________________________________
%
% See also: KbQueueCreate, KbQueueStart, KbQueueStop, KbQueueCheck,
%            KbQueueWait, KbQueueFlush, KbQueueRelease

% 8/19/07    rpw  Wrote it.
% 8/23/07    rpw  Modifications to add KbQueueFlush
% 5/14/12    mk   Small fixes: Use 1 msec wait interval.
% 7/29/16    dcn  Added forWhat and untilTime arguments mirroring KbWait
% 8/31/16    dcn  Additional help note about changed KbQueueFlush
%                 recommendation

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
% It is implicit in invoking this function that the queue should be running
% and it is potentially problematic if it is not since the function will
% never return, therefore, go ahead and start the queue if it isn't running
KbQueueStart(deviceIndex);

if nargin < 2 || isempty(forWhat)
    forWhat = 0;
end

if nargin < 3 || isempty(untilTime)
    untilTime = inf;
end


% Wait for keystroke?
if forWhat >= 2
    % Wait for keystroke, ie., first make sure all keys are released, then
    % wait for a keypress:
    
    % Wait until all keys are released
    KbQueueWaitInternal(deviceIndex, 1, untilTime, true);
    
    if forWhat == 2
        % Now just go on with forWhat = 0, i.e., wait for keypress:
        secs = KbQueueWaitInternal(deviceIndex, 0, untilTime, false);
    else
        % Wait for keypress (we want time of the press, not the later
        % release):
        secs = KbQueueWaitInternal(deviceIndex, 0, untilTime, false);
        
        % Wait for key release.
        KbQueueWaitInternal(deviceIndex, 1, untilTime, false);
    end
else
    secs = KbQueueWaitInternal(deviceIndex, forWhat, untilTime, true);
end



% helper function that provides some extra control to implement higher
% forWhats correctly
function secs = KbQueueWaitInternal(deviceIndex, forWhat, untilTime, qFirst)
secs = -inf;
while secs < untilTime
    [~,~,~,p,r] = KbQueueCheck(deviceIndex);
    if ~forWhat && any(p>r) % waiting for key down
        % if for any key, last down was later than last up, it is currently
        % down
        secs=min(p(p>r));
        return;
    elseif forWhat && any(r>p) % waiting for key up
        % if for none of the keys last down was later than last up, no keys
        % are currently down. also nothing down if all fields are 0
        secs=max(r(r>p));
        return;
    elseif qFirst
        % special case for first iteration: if no key event in buffer at
        % all, we'll have to assume all keys are up. if waiting for keys
        % up, return now
        if forWhat && ~any(p) && ~any(r)
            return;
        end
        qFirst = false;
    end
    
    % Wait for 1 msec to prevent system overload:
    secs = WaitSecs('Yieldsecs', 0.001);
end
