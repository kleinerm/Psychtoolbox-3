function err=DaqAOut(device,channel,v)
% err=DaqAOut(device,channel,v)
% USB-1208FS analog output.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% "channel" is 0 or 1.
% "v" is a value in the range 0 to 1, which will produce an output voltage
%       in the range 0 to 4.095 V.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid,
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.

% 4/15/05 dgp Wrote it.

% if ~ismember(channel,0:1)
% 	error('"channel" must be 0 or 1.');
% end
report=uint8(zeros(1,4));
report(2)=channel; % channel
v=max(0,min(1,v));
v=round(v*65535); % 16-bit unsigned. Bottom 4 bits are ignored.
% v=max(256,min(65535,v));
report(3)=bitand(v,255);  % low byte
report(4)=bitshift(v,-8); % high byte
% hex2dec('14') is 20
err=PsychHID('SetReport',device,2,20,report); % write analog output
if err.n
    fprintf('DaqAOut error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end


