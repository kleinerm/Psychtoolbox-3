function err=DaqBlinkLED(daq)
% err=DaqBlinkLED(DeviceIndex)
% USB-1208FS: blink LED.
% "DeviceIndex" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest.
%
% 4/15/05 dgp Wrote it.
% 11/13/07  mpr Tested it on USB-1608FS called by Matlab 2007b from a Mac
%                   Pro running Leopard.  Worked with no changes! 
% 1/10/08   mpr worked to get improved internal consistency (changed
%                   "device" to "daq", fixed "TestDaq" and "TestPsychHid"


% DaqBlinkLED
err=PsychHID('SetReport',daq,2,64,uint8(64)); % Blink LED
if err.n
    fprintf('DaqBlinkLED SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

return;
