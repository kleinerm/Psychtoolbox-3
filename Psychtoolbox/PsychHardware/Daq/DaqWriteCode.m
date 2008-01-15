function err=DaqWriteCode(daq,address,data)
% err=DaqWriteCode(DeviceIndex,address,data)
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
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "address" is the 24-bit start address for the write.
% "data" is a vector with length up to 32, one element per byte.
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest.
%
% 4/15/05 dgp Wrote it.
% 12/2x/07  mpr tested it and found it wanting
% 1/11/08   mpr   swept through attempting to improve consistency across
%                   daq functions
%
% I have no current plans to make sure this code works, but on 1/8/08 I
% discovered it might work as is...  According to a comment in the source files
% written by the linux people, this function doesn't actually cause the program
% memory to be written.  It puts an image of the code in external SRAM, and a
% subsequent call to UpdateCode must be made to write the program into the
% device's flash memory.  The command code for the UpdateCode command is 84,
% which Denis did not list for the 1208FS.  I have not looked to see if that is
% a difference between the 1208 and 1608 or if this was just something Denis
% never discovered because he didn't look for it.  Anyways, I have no current
% plans to create a PsychHID wrapper for the UpdateCode command, so if this
% functionality is something you want, then that's what you'll probably need to
% do to get things working.  And if you do that, you probably want to invest 
% some time in figuring out the ReadChecksum (command code 82) for verification 
% that program memory is correct. -- mpr

fprintf(['\nDaqWriteCode is non-functional so far as I know.  It will run without errors\n' ...
         'as long as your device is in program mode when the function is called.  However,\n' ...
         'in my tests it never did anything.  Denis claims he was not foolish enough to even\n' ...
         'test the code.  DaqPrepareDownload and DaqReadCode appear to function as they\n' ...
         'should, so I don''t know why this function doesn''t work.  But like Denis, I don''t\n' ...
         'see a need for it, so I have not spent a lot of time playing with it.  Type:\n\n' ...
         '     help DaqWriteCode\n\nfor a bit more information-- MPR\n\n']);
error('DaqWriteCode has never written anything... perhaps it is a member of the Writers Guild of America.');

TheStatus = DaqGetStatus(daq);
if isempty(TheStatus)
  error('I could not get the status of your device.  Something is really hinky');
end
if isstruct(TheStatus)
  if isfield(TheStatus,'program')
    if ~TheStatus.program
      error('Device is not in program mode.  Run DaqPrepareDownload(DeviceIndex) first.');
    end
  else
    error('Improper structure from DaqGetStatus.  Don''t know what to do!');
  end
else
  error('Improper return type from DaqGetStatus.  Don''t know what to do!');
end

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
report(1)=81;
report(2)=bitand(address,255);
address=bitshift(address,-8);
report(3)=bitand(address,255);
report(4)=bitshift(address,-8);
report(5)=length(data);
report(6:end)=data;
err=PsychHID('SetReport',daq,2,81,uint8(report)); % WriteCode
if err.n
    fprintf('DaqWriteCode SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

CodeRead = DaqReadCode(daq,address,length(data));

return
