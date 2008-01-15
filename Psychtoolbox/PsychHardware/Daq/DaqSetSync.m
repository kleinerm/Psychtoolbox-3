function err=DaqSetSync(daq,type)
% err=DaqSetSync(DeviceIndex,type)
% USB-1208FS: Configure sync input/output. 
% "DeviceIndex" is a small integer, the array index specifying which HID
%         device in the array returned by PsychHID('Devices') is interface
%         0 of the desired USB-1208FS box.
% "type" is 0 for master, 1 for slave with continuous clock, or 2 for slave
%         with gated clock. 
% This command configures the sync signal.  The sync signal may be used to
% synchronize the analog input scan of multiple devices.  When multiple
% devices are to be used, one device is selected as the master and the rest
% as slaves.  The sync signal of all devices must be wired together.  The
% master will output a pulse every sample, and all of the devices will
% acquire their samples simultaneously. This may also be used to pace one
% or more devices from an external TTL/CMOS clock signal (max rate 50 kHz.)
% This may also be used with an external trigger; the external trigger
% signal should be brought to the master device, and all devices will begin
% sampling when the master is triggered. If a device is configured as a
% slave, it will not acquire data when given an AInScan command until it
% detects a pulse on the sync input. If configured as a slave with a
% continuous clock, an additional sync pulse is required to set up the
% AInScan.  If configured as a slave with a gated clock the additional sync
% pulse is not required.  However, if a sync pulse is received while the
% AInScan setup is being performed by the device, improper operation may
% result.  This is intended for use when synchronizing with another USB-1208FS,
% where the sync signal will not be present until the master device has
% been issued an AInScan command. The device will switch the SYNC pin to
% the appropriate input/output state when this command is received.
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest.

% 4/15/05 dgp Wrote it.
% 1/11/08 mpr swept through attempting to improve consistency across daq
%               functions

if ~ismember(type,0:2)
	error('"type" must be in range 0 to 2.');
end
err=PsychHID('SetReport',daq,2,67,uint8([67 type])); % SetSync
if err.n
    fprintf('SetSync error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;

