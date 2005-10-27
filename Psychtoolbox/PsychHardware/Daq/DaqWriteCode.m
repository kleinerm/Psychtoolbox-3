function err=DaqWriteCode(device,address,data)
% err=DaqWriteCode(device,address,data)
% USB-1208FS: Write program memory. This command writes to the program
% memory in the device.  This command is not accepted unless the device is
% in update mode (see PrepareDownload).  The WriteCode command will
% normally be used when downloading a new hex file, so it supports the
% memory ranges that may be found in the hex file.
% The address ranges are:
% 0x000000 - 0x007AFF: FLASH program memory
% 0x200000 - 0x200007: ID memory (serial number is stored here)
% 0x300000 - 0x30000F: CONFIG memory (processor configuration data)
% 0xF00000 - 0xF03FFF: EEPROM memory
% When writing to FLASH program memory, length(data) must be 32 and the
% device must receive data in successive 32-byte chunks starting on a
% 64-byte boundary. When writing to other kinds of memory, length(data) can
% be any number of bytes up to the maximum (32).
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "address" is the 24-bit start address for the write.
% "data" is a vector with length up to 32, one element per byte.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

if length(data)>32
    error('"data" vector is too long.');
end
if numel(data)~=length(data)
    error('"data" must be a vector.');
end
if isempty(data)
    error('"data" vector is empty.');
end
if any(~ismember(data,0:255))
    error('"data" values must be in the range 0 to 255.');
end
report=zeros(1,5+length(data));
rep;ort(1)=81;
report(2)=bitand(address,255);
address=bitshift(address,-8);
report(3)=bitand(address,255);
report(4)=bitshift(address,-8);
report(5)=length(data);
report(6:end)=data;
err=PsychHID('SetReport',device,2,81,uint8(report)); % WriteCode
if err.n
    fprintf('DaqWriteCode SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end


