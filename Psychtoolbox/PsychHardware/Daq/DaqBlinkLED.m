function err=DaqBlinkLED(device)
% err=DaqBlinkLED(device)
% USB-1208FS: blink LED.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

% DaqBlinkLED
err=PsychHID('SetReport',device,2,64,uint8(64)); % Blink LED
if err.n
    fprintf('DaqBlinkLED SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

