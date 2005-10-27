function err=DaqMemWrite(device,address,data)
% err=DaqMemWrite(device,address,data)
% USB-1208FS: Write memory. This command writes to the nonvolatile EEPROM
% memory on the device. The nonvolatile memory is used to store
% calibration coefficients, system information, and user data.  Locations
% 0x000 to 0x07F are reserved for firmware use and may not be written.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "address" is the 16-bit start address for the write.
% "data" is a vector with length up to 59, one element per byte.
% See also DaqWriteCode, DaqMemRead, Daq, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

if numel(data)~=length(data)
    error('"data" must be a vector.');
end
if length(data)>59
    error('"data" vector is too long.');
end
if isempty(data)
    error('"data" vector is empty.');
end
if any(~ismember(data,0:255))
    error('"data" values must be in the 8-bit range 0 to 255.');
end
report=zeros(1,4+length(data));
report(1)=49;
report(2)=bitand(address,255);
report(3)=bitshift(address,-8);
report(4)=length(data);
report(5:end)=data;
err=PsychHID('SetReport',device,2,49,uint8(report)); % MemWrite
if err.n
    fprintf('DaqMemWrite SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
