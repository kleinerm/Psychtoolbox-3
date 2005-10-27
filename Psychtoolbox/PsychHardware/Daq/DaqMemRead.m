function data=DaqMemRead(device,address,bytes)
% data=DaqMemRead(device,address,bytes)
% USB-1208FS: Read memory. This command reads data from the configuration
% memory (EEPROM).  All of the memory may be read.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "address" is the 16-bit start address for the read.
% "bytes" is the number of bytes to be read, up to a maximum of 62.
% See also DaqWriteCode, DaqMemWrite, Daq, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

if ~ismember(bytes,1:62)
    error('Can''t read more than 62 bytes.');
end
err=PsychHID('ReceiveReports',device);
err=PsychHID('ReceiveReportsStop',device);
[reports,err]=PsychHID('GiveMeReports',device);
report=zeros(1,4);
report(1)=48;
report(2)=bitand(address,255);
report(3)=bitshift(address,-8);
report(4)=0; % unused
report(5)=bytes;
err=PsychHID('SetReport',device,2,48,uint8(report)); % MemRead
if err.n
    fprintf('DaqMemRead SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
[data,err]=PsychHID('GetReport',device,1,48,bytes+1); % MemRead
if err.n
    fprintf('DaqMemRead GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
err=PsychHID('ReceiveReportsStop',device);
