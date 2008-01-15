function err=DaqAInStop(daq)
% err=DaqAInStop(DeviceIndex)
% USB-1208FS stop analog input scan. You probably will never need to
% explicitly call this function because it's already called on your behalf
% by DaqAInScan and DaqAInScanEnd.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, DaqAInScan.

% 4/15/05 dgp Wrote it.
% 1/13/08 mpr swept through to make terminology more consistent across daq functions

err=PsychHID('SetReport',daq,2,18,uint8(18)); % AInStop
if err.n
    fprintf('DaqAInStop SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;
