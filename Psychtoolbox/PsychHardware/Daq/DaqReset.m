function err=DaqReset(device)
% err=DaqReset(device)
% Assuming that something's wrong with the USB-1208FS or our communication
% with it, we re-enumerate in order to re-establish communication. Then we
% send the reset command to ask the USB-1208FS to reset itself. Then we
% re-enumerate again to re-establish communication once more.
% 
% To avoid problems caused by CLEAR PsychHID, we recommend that, instead of 
% calling DaqReset, you unplug and reinsert the USB cable of your
% USB-1208FS and quit and restart MATLAB. In our experience that
% combination always restores normal communication.
% 
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

fprintf('Resetting USB-1208FS.\n');
clear PsychHID; % flush current enumeration  (list of devices)
devices=PsychHID('devices'); % enumerate again
daq=[];
for i=1:length(devices)
    if streq(devices(i).product,'USB-1208FS')
        if isempty(daq)
            daq=i;
        elseif ~streq(devices(i).serialNumber,devices(daq(end)).serialNumber)
            daq(end+1)=i;
        end
    end
end
if isempty(daq)
    error('Sorry, couldn''t find a USB-1208FS.');
end
if ~any(ismember(device,daq))
    device=daq(1);
end
% Reset. Ask the USB-1208FS to reset its USB interface.
% CAUTION: Immediately after RESET, all commands fail, returning error
% messages saying the command is unsupported (0xE00002C7) or the device is
% not responding (0xE00002ED) or not attached (0xE00002D9). To restore
% communication we must flush the current enumeration and re-enumerate the
% HID-compliant devices.
% fprintf('Reset. Ask the USB-1208FS to reset its USB interface.\n');
err=Reset(device);
% fprintf('(Reestablishing communication: Flushing current enumeration. ');
clear PsychHID; % flush current enumeration  (list of devices)
% fprintf('Re-enumerating. ... ');
devices=PsychHID('devices'); % enumerate again
daq=[];
for i=1:length(devices)
    if streq(devices(i).product,'USB-1208FS')
        if isempty(daq)
            daq=i;
        elseif ~streq(devices(i).serialNumber,devices(daq(end)).serialNumber)
            daq(end+1)=i;
        end
    end
end
% fprintf('Done.)\n');
return

function err=Reset(device)
% err=Reset(device)
% USB-1208FS: Ask device to reset its USB interface.
err=PsychHID('SetReport',device,2,65,uint8(65)); % Reset
if err.n
    fprintf('Reset SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
return

