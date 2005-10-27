function data=DaqDIn(device)
% data=DaqDIn(device)
% USB-1208FS: Read digital ports. This command reads the current state of
% the DIO ports.  The return value will be the value seen at the port pins.
% data(1) is the 8-bit value read from port A.
% data(2) is the 8-bit value read from port B.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.

% 4/15/05 dgp Wrote it.

err=PsychHID('ReceiveReports',device);
[reports,err]=PsychHID('GiveMeReports',device);
err=PsychHID('SetReport',device,2,3,uint8(0)); % DIn: Read digital ports
if err.n
    fprintf('DaqDIn SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
[report,err]=PsychHID('GetReport',device,1,3,3); % Get report
if err.n
    fprintf('DaqDIn GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
if length(report)==3 && report(1)==3
    % 2 bytes
    data=double(report(2:3));
else
    data=[];
end
err=PsychHID('ReceiveReportsStop',device);
