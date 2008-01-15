function err=DaqSetTrigger(daq,rising)
% err=DaqSetTrigger(DeviceIndex,rising)
% USB-1208FS: Configure the external trigger. This function configures the
% external trigger for analog input. The trigger may be configured to
% respond to either a logic rising edge ("rising"=1) or falling edge input
% ("rising"=0). Once the trigger is received, the analog input will proceed
% as configured. options.trigger must be 1 in the DaqAInScan command
% to utilize this feature. 
% "DeviceIndex" is a small integer, the array index specifying which HID
%         device in the array returned by PsychHID('Devices') is interface
%         0 of the desired USB-1208FS box.
% "rising" selects the desired edge type (0 = falling, 1 = rising) for the 
%         external trigger.
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest.

% 4/15/05 dgp Wrote it.
% 1/11/08 mpr swept through attempting to improve consistency across daq
%               functions

if ~ismember(rising,0:1)
	error('"rising" must be 0 or 1.');
end
err=PsychHID('SetReport',daq,2,66,uint8([66 rising])); % SetTrigger
if err.n
    fprintf('SetTrigger error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;
