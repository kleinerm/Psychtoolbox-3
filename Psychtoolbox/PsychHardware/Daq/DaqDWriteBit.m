function err=DaqDWriteBit(daq,BitNumber,TheValue)
% err=DaqDReadBit(DeviceIndex,BitNumber,value)
% USB-1608FS: Write bit to digital port. 
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1608FS box.
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
% BitNumber should range from 0:7 specifying the channel whose bit you want set;
% 0 corresponds to screw terminal 21, 1 corresponds to screw terminal 23, etc.
% "value" should be 0 or 1.
%
% BitNumber can be a vector in order to write multiple bits at once.  To
% implement this, value must be a vector the same length as BitNumber, or it
% must be a scalar in which case all designated bits will be given that value.
%
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest,
% DaqDConfigPortBit, DaqDReadBit, DaqDeviceIndex, DaqDIn, DaqDOut.
%
% 12/17/07 mpr scavenged code from DaqDOut and edited it to make this
% 1/11/08  mpr  swept through attempting to improve consistency across daq
%                   functions

if length(BitNumber) > 1
  if numel(TheValue) == 1
    TheValue = TheValue*ones(size(BitNumber));
  end
  if length(TheValue) ~= length(BitNumber)
    error('Mismatched inputs; size of value must be either scalar or size of BitNumber');
  end
  for k=1:length(BitNumber)
    err(k) = DaqDWriteBit(daq,BitNumber(k),TheValue(k));
  end
  return;
end

err=PsychHID('SetReport',daq,2,6,uint8([6 BitNumber TheValue])); % DWriteBit
if err.n
    fprintf('DaqDOut error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;
