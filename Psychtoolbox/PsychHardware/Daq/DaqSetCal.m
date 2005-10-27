function err=DaqSetCal(device,on)
% err=DaqSetCal(device,on)
% USB-1208FS set CAL output.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "on" is 1 for on (+2.5 V), and 0 for off (0 V).
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

if ~ismember(on,0:1)
	error('"on" must be 0 or 1.');
end
err=PsychHID('SetReport',device,2,69,uint8([69 on])); % SetCal
if err.n
    fprintf('DaqSetCal SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

