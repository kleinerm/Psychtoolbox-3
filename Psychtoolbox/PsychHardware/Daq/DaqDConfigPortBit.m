function err=DaqDConfigPortBit(daq,BitNumber,direction)
% err=DaqDConfigPortBit(DeviceIndex,BitNumber,direction)
% USB-1608FS: Configure digital port. This command sets the direction of
% the DIO port to input or output.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "BitNumber" ranges from 0:7 and can be a vector if you want to configure 
%       multiple ports at the same time.
% "direction" 0 = output, 1 = input; must be either a scalar or a vector of the 
%       same length as BitNumber
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest, DaqDReadBit, 
%       DaqDWriteBit, DaqDConfigPort, DaqDIn, DaqDOut.
%
% ******************************************************************************
% *                                                                            *
% * If your Daq is a USB-1208FS or USB-1408FS, this code has not been tested   *
% * probably will not run on your device.  The USB-1608FS has only one DIO     *
% * port and that is what this code was written to expect.  To make it run on  *
% * a 12 or 14 bit device, you should fix this to take an additional input     *
% * (the portnumber) and to continue to behave as is for the 16-bit device.    *
% * An example of how to do that can be found in DaqDConfigPort.  -- mpr       *
% *                                                                            *
% ******************************************************************************
% 
% Code has not been tested on USB-1208FS or USB-1408FS.  If you have one of
% those devices, you should modify this code to accept portnumber as an
% additional input parameter.  See DaqDConfigPort for an example of how to deal
% with the added input.
%
% 12/17/07  mpr scavenged and modified code from DaqDConfigPort
% 1/11/08   mpr swept through trying to improve consistency across daq
%                 functions

if length(BitNumber) > 1
  if numel(direction) == 1
    direction = direction*ones(size(BitNumber));
  end
  if ~all(size(BitNumber) == size(direction))
    error('Dimension mismatch; direction must be either scalar or vector of same length as BitNumber');
  end
  for k=1:length(BitNumber)
    err(k) = DaqDConfigPortBit(daq,BitNumber(k),direction(k));
  end
  return;
end

if ~ismember(direction,0:1)
	error('"direction" must be 0 (out) or 1 (in).');
end
report=uint8([2 BitNumber direction]);
err=PsychHID('SetReport',daq,2,2,report); % Configure digital port bit.
if err.n
    fprintf('DaqDConfigPortBit error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
return
