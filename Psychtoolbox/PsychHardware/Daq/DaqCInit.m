function err=DaqCInit(daq)
% err=DaqCInit(DeviceIndex)
% USB-1208FS: Initialize counter to zero.
% "DeviceIndex" is a small integer, the array index specifying which HID
%         device in the array returned by PsychHID('Devices') is interface 0
%         of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest.
%
% 4/15/05 dgp Wrote it.
% 12/20/07  mpr   tested it on 1608FS and then made input argument optional
% 1/10/08   mpr   swept through trying to improve consistency across Daq
%                     functions

if ~nargin | isempty(daq)
  daq=DaqFind;
end

err=PsychHID('SetReport',daq,2,32,uint8(32)); % CInit
if err.n
    fprintf('CInit SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;
