function err=DaqDOut(device,port,data)
% err=DaqDOut(device,port,data)
% USB-1208FS: Write digital port. This command writes data to the DIO port
% bits that are configured as outputs.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "port" 0 = port A, 1 = port B
% "data" 8-bit value, 0 to 255.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.

% 4/15/05 dgp Wrote it.

% if ~ismember(port,0:1)
% 	error('"port" must be 0 or 1.');
% end
% if ~ismember(data,0:255)
% 	error('"data" must be in range 0:255.');
% end
err=PsychHID('SetReport',device,2,4,uint8([0 port data])); % DOut
if err.n
    fprintf('DaqDOut error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
