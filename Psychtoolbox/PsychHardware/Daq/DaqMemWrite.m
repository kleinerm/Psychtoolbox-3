function err=DaqMemWrite(daq,address,data)
% err=DaqMemWrite(DeviceIndex,address,data)
% USB-1208FS: Write memory. This command writes to the nonvolatile EEPROM
% memory on the device. The nonvolatile memory is used to store
% calibration coefficients, system information, and user data.  Locations
% 0x000 to 0x07F are reserved for firmware use and may not be written.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "address" is the 16-bit start address for the write.
% "data" is a vector with length up to 59, one element per byte.
% See also DaqWriteCode, DaqMemRead, Daq, DaqTest, PsychHIDTest.

% 4/15/05 dgp Wrote it.
% 12/14/07  mpr added error checking for address
% 1/11/08   mpr swept through trying to improve consistency across daq
%                   functions

if address > 1023
  warning('EEPROM of USB-1208FS and USB-1608FS is only 1024 bytes.  Larger values wrap around.');
  % In my tests, the wrap was automatic (i.e., an address of 1024 is equivalent
  % to an address of 0), but I'm going to hard code it here anyways so that I
  % know which part of EEPROM space the user will be hitting in the following
  % tests. -- mpr
  while address > 1023
    address = address-1024;
  end
end

% In my tests, trying to write to the "Reserved" memory space had no
% demonstrable effect even though the function supposedly ran with no errors.
% Nevertheless, it seems dangerous to me that someone might be allowed to
% write or even just allowed to think they can write to the reserved space.  So
% I disallow it.  -- mpr
if address < 128
  warning('Attempt to write to Reserved section of DAQ''s EEPROM.  Doing nothing.');
  err.n = 0;
  err.name = '';
  err.description = '';
  return;
% Writing to the calibration section of the memory worked in my tests, and I can
% imagine it to be something people would want to do.  But let's make sure no
% one does it by accident. -- mpr
elseif address < 512
  IShouldWriteCalData = TwoStateQuery('Detected effort to re-write DAQ calibration data... Should I allow it?');
  if IShouldWriteCalData < 1
    disp('Per my understanding of your choice, I did not re-write the calibration data.');
    err.n = 0;
    err.name = '';
    err.description = '';
    return;
  end
end

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
err=PsychHID('SetReport',daq,2,49,uint8(report)); % MemWrite
if err.n
    fprintf('DaqMemWrite SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;
