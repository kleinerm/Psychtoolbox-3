function data=DaqDIn(daq,NumberOfPorts)
% data=DaqDIn([DeviceIndex],[NumberOfPorts])
% USB-1208FS: Read digital ports. This command reads the current state of
% the DIO ports.  The return value will be the value seen at the port pins.
% data(1) is the 8-bit value read from port A.
% data(2) is the 8-bit value read from port B.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
%
% USB-1608FS: There is only one port, so only one meaningful return value.  
% First argument is optional if you have only one DAQ device, but code will
% run faster if DeviceIndex is specified.  Second argument is optional; if
% it is not specified, software will attempt to determine if it should be 1
% or 2 based on the type of device.  Specifying it explicitly speeds up
% code about 1 to 2  milliseconds per call on a dual core MacI with 2.67
% GHz processors. -- mpr 
%
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest.
% DaqDeviceIndex, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.
%
% 4/15/05 dgp Wrote it.
% 12/18/07  mpr   added second argument and made first optional
% 1/11/08   mpr   swept through trying to improve consistency across daq
%                     functions

if ~nargin | isempty(daq)
  daq=DaqFind;
end

if nargin < 2 | isempty(NumberOfPorts)
  TheDevices = PsychHID('Devices');
  if strcmp(TheDevices(daq).product(1:6),'USB-16')
    NumberOfPorts = 1;
  else
    NumberOfPorts = 2;
  end
end

err=PsychHID('ReceiveReports',daq);
[reports,err]=PsychHID('GiveMeReports',daq);

err=PsychHID('SetReport',daq,2,3,uint8(0)); % DIn: Read digital ports

if err.n
    fprintf('DaqDIn SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
[report,err]=PsychHID('GetReport',daq,1,3,NumberOfPorts+1); % Get report
if err.n
    fprintf('DaqDIn GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
if length(report)==NumberOfPorts+1 && report(1)==3
    % 2 bytes for 1208FS or 1408FS, but only 1 byte for 1608FS
  data=double(report(2:end));
else
  data=[];
end

err=PsychHID('ReceiveReportsStop',daq);

return;
