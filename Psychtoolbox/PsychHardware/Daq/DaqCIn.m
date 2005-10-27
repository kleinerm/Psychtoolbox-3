function count=DaqCIn(device)
% count=DaqCIn(device)
% USB-1208FS: Read counter.
% This function reads the 32-bit event counter in the device. This counter
% tallies the transitions of an external input attached to the CTR pin on
% the screw terminal of the device.
% "count" is the 32-bit value.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

err=PsychHID('ReceiveReports',device);
err=PsychHID('ReceiveReportsStop',device);
[reports,err]=PsychHID('GiveMeReports',device);
err=PsychHID('SetReport',device,2,33,uint8(33)); % CIn
if err.n
    fprintf('DaqCIn SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
[report,err]=PsychHID('GetReport',device,1,33,5); % get report
if err.n
    fprintf('DaqCIn GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
if length(report)==5
    % 4 bytes
    report=double(report);
    count=report(2)+256*(report(3)+256*(report(4)+256*report(5)));
else
    count=[];
end
err=PsychHID('ReceiveReportsStop',device);


