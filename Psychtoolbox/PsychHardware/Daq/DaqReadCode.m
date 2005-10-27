function data=DaqReadCode(device,address,bytes)
% data=DaqReadCode(device,address,bytes)
% USB-1208FS: Read program memory.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "address" is a 24-bit address.
% "bytes" is the number of bytes to read (up to 62);
% See also DaqWriteCode, Daq, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

if ~ismember(bytes,1:62)
    error('Can''t read more than 62 bytes.');
end
err=PsychHID('ReceiveReports',device);
err=PsychHID('ReceiveReportsStop',device);
[reports,err]=PsychHID('GiveMeReports',device);
report=zeros(1,5);
report(1)=85;
report(2)=bitand(address,255);
address=bitshift(address,-8);
report(3)=bitand(address,255);
report(4)=bitshift(address,-8);
report(5)=bytes;
err=PsychHID('SetReport',device,2,85,uint8(report)); % ReadCode
if err.n
    fprintf('DaqReadCode SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
[data,err]=PsychHID('GetReport',device,1,85,bytes+1); % ReadCode
if err.n
    fprintf('DaqReadCode GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
err=PsychHID('ReceiveReportsStop',device);
return

