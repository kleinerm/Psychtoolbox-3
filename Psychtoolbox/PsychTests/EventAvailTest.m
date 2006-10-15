% EventAvailTest

WaitSecs(1);
[isThere, eventName]=EventAvail('keyDown', 'autoKey', 'mouseDown','mouseUp');
if isThere
    eventName
    break
end


