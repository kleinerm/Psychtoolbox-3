function err=DaqCInit(device)
% err=DaqCInit(device)
% USB-1208FS: Initialize counter to zero.
% "device" is a small integer, the array index specifying which HID
%         device in the array returned by PsychHID('Devices') is interface 0
%         of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

err=PsychHID('SetReport',device,2,32,uint8(32)); % CInit
if err.n
    fprintf('CInit SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end


