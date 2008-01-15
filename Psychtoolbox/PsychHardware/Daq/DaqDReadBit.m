function BitValue=DaqDReadBit(daq,BitNumber)
% BitValue=DaqDReadBit(DeviceIndex,BitNumber)
% USB-1608FS: Write digital port. This command writes data to the DIO port
% bits that are configured as outputs.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1608FS box.
% "BitNumber" an integer from 0 to 8 specifying which bit to read
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest.
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.
%
% ******************************************************************************
% *                                                                            *
% * If your Daq is a USB-1208FS or USB-1408FS, this code has not been tested;  *
% * it probably will not run on your device.  The USB-1608FS has only one DIO  *
% * port and that is what this code was written to expect.  To make it run on  *
% * a 12 or 14 bit device, you should fix this to take an additional input     *
% * (the portnumber) and to continue to behave as is for the 16-bit device.    *
% * An example of how to do that can be found in DaqDConfigPort.  -- mpr       *
% *                                                                            *
% ******************************************************************************
% 
% I suspect that to get this code to work for a 1208FS or 1408FS you would have
% to modify the SetReport call to add the port number (0 or 1) before 
% BitNumber.  Hopefully that would be all it would take, but since I don't 
% have one of those devices to test, I have not implemented that fix.  Look at 
% the code in DaqDOut to see how you might code something that tested for
% whether your Daq has two DIO ports, and how you'd handle input and report to
% accommodate the added port.
%
% BitNumber can be a vector so that you can read multiple bits at once.
%
% 12/xx/07 mpr scavenged code from DaqDIn and converted it to this.
% 1/11/08  mpr  swept through attempting to improve consistency across daq
%                   functions

if length(BitNumber) > 1
  BitValue = NaN*ones(1,length(BitNumber));
  for k=1:length(BitNumber)
    BitValue(k) = DaqDReadBit(daq,BitNumber(k));
  end
  return;
end

err=PsychHID('ReceiveReports',daq);
[reports,err]=PsychHID('GiveMeReports',daq);
err=PsychHID('SetReport',daq,2,5,uint8([5 BitNumber])); % DReadBit
if err.n
    fprintf('DaqDReadBit SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
[report,err]=PsychHID('GetReport',daq,1,5,2); % Get report
if err.n
    fprintf('DaqDReadBit GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
BitValue=double(report(2));
err=PsychHID('ReceiveReportsStop',daq);

return;
