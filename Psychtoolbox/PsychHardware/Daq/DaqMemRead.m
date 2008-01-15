function [data,TheErrors]=DaqMemRead(daq,address,bytes)
% [data,[errorstructure]]=DaqMemRead(DeviceIndex,address,bytes)
% USB-1208FS: Read memory. This command reads data from the configuration
% memory (EEPROM).  All of the memory may be read.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "address" is the 16-bit start address for the read.
% "bytes" is the number of bytes to be read, up to a maximum of 62.
%
% Several functions called by this function may produce errors, so
% errorstructure is a vector.  It is only returned if the function is called
% with two output arguments.
% See also DaqWriteCode, DaqMemWrite, Daq, DaqTest, PsychHidTest.

% 4/15/05 dgp Wrote it.
% 12/14/07  mpr added optional second output 
% 1/11/08   mpr swept through attempting to improve consistency across daq
%                   functions

% I have not found a reason for the 62 byte limitation... -- mpr
if ~ismember(bytes,1:62)
    error('Can''t read more than 62 bytes.');
end
err=PsychHID('ReceiveReports',daq);
if nargout > 1
  TheErrors = BuildErrVector([],err);
end
err=PsychHID('ReceiveReportsStop',daq);
if nargout > 1
  TheErrors = BuildErrVector(TheErrors,err);
end
[reports,err]=PsychHID('GiveMeReports',daq);
if nargout > 1
  TheErrors = BuildErrVector(TheErrors,err);
end
report=zeros(1,4);
report(1)=48;
report(2)=bitand(address,255);
report(3)=bitshift(address,-8);
report(4)=0; % unused
report(5)=bytes;
err=PsychHID('SetReport',daq,2,48,uint8(report)); % MemRead

if nargout > 1
  TheErrors = BuildErrVector(TheErrors,err);
end
if err.n
    fprintf('DaqMemRead SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
[data,err]=PsychHID('GetReport',daq,1,48,bytes+1); % MemRead
if nargout > 1
  TheErrors = BuildErrVector(TheErrors,err);
end
if err.n
    fprintf('DaqMemRead GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
err=PsychHID('ReceiveReportsStop',daq);
if nargout > 1
  TheErrors = BuildErrVector(TheErrors,err);
end

return;

function TheErrors = BuildErrVector(TheErrors,NewError)
if ~isfield(NewError,'reportLength')
  NewError.reportLength = NaN;
end
if ~isfield(NewError,'reportTime')
  NewError.reportTime = NaN;
end
if isempty(TheErrors)
  TheErrors = NewError;
else
  TheErrors(end+1) = NewError;
end

return;
