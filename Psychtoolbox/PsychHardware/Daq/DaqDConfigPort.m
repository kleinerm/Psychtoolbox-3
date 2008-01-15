function err=DaqDConfigPort(daq,port,direction)
% err=DaqDConfigPort(DeviceIndex,port,direction)
% USB-1208FS: Configure digital port. This command sets the direction of
% the DIO port to input or output.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "port" 0 = port A, 1 = port B
% "direction" 0 = output, 1 = input
%
% USB-1608FS: has only one port, so second argument is meaningless.  If three
% arguments are passed and device is a 1608FS, second argument is ignored.
% Function could be made more efficient if a separate one were written for
% 1608FS, but I'm guessing this function will never be used at a time when 
% maximal speed is an overarching requirement.  
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHidTest.
%
% 4/15/05 dgp Wrote it.
% 12/18/07  mpr extended for 1608
% 1/11/08   mpr swept through trying to improve consistency across daq
%                 functions

TheDevices = PsychHID('Devices');
if strcmp(TheDevices(daq).product(5:6),'16')
  Is1608=1;
  if nargin == 2
    direction = port;
  end
else
  Is1608=0;
end

% Denis(?) commented this out for some reason...
% if ~ismember(port,0:1)
% 	error('"port" must be 0 or 1.');
% end
if ~ismember(direction,0:1)
	error('"direction" must be 0 (out) or 1 (in).');
end
if Is1608
  report = uint8([1 direction]);
else
  report=uint8([1 port direction]);
end
err=PsychHID('SetReport',daq,2,1,report); % Configure digital port.
if err.n
    fprintf('DaqDConfigPort error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
return
