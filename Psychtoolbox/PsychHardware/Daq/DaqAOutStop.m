function err=DaqAOutStop(device)
% err=DaqAOutStop(device)
% USB-1208FS stop analog output scan.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, DaqAOutScan, TestDaq, 
% TestPsychHid.

% 4/15/05 dgp Wrote it.

err=PsychHID('SetReport',device,2,22,uint8(22)); % AOutStop
if err.n
    fprintf('DaqAOutStop SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
