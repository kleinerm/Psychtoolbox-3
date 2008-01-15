function err=DaqWriteSerialNumber(daq,serialstring)
% err=DaqWriteSerialNumber(DeviceIndex,serialstring)
% USB-1208FS: Write a new serial number (an 8-character string) to the
% device. The serial number consists of 8 bytes, typically ASCII numeric or
% hexadecimal digits (i.e. '00000001'.)  The new serial number will be
% programmed but not used until hardware reset. As far as I know, the only
% use of the serial number is to distinguish multiple USB-1208FS devices
% attached to the same computer.
% See also DaqWriteCode, Daq, DaqTest, PsychHIDTest.

% 4/15/05 dgp Wrote it.
% 1/11/08 mpr swept through attempting to improve consistency across daq
%               functions

report=uint8([83 serialstring]);
err=PsychHID('SetReport',daq,2,83,report); % WriteSerialNumber
if err.n
    fprintf('DaqWriteSerialNumber error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;
