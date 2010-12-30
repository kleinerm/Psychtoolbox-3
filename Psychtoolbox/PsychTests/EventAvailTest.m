% EventAvailTest

% This is a no-op as we do not have EventAvail
% on PTB-3
fprintf('EventAvail unsupported on PTB-3\n');
return;

WaitSecs(1);
[isThere, eventName]=EventAvail('keyDown', 'autoKey', 'mouseDown','mouseUp');
if isThere
    eventName
    break
end
