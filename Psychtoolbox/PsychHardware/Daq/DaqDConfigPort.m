function err=DaqDConfigPort(device,port,direction)
% err=DaqDConfigPort(device,port,direction)
% USB-1208FS: Configure digital port. This command sets the direction of
% the DIO port to input or output.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "port" 0 = port A, 1 = port B
% "direction" 0 = output, 1 = input
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

if ~ismember(port,0:1)
	error('"port" must be 0 or 1.');
end
if ~ismember(direction,0:1)
	error('"direction" must be 0 (out) or 1 (in).');
end
report=uint8([1 port direction]);
err=PsychHID('SetReport',device,2,1,report); % Configure digital port.
if err.n
    fprintf('DaqDConfigPort error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
return
