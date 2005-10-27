function status=DaqGetStatus(device)
% status=DaqGetStatus(device)
% USB-1208FS: Retrieve device status.
% "status.master" =0 sync slave; 1 sync master.
% "status.rising" =0 trigger on falling edge; 1 trigger on rising edge.
% "status.program" =1 program memory update mode.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0
%       of the desired USB-1208FS box.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

err=PsychHID('ReceiveReports',device);
err=PsychHID('ReceiveReportsStop',device);
err=PsychHID('GiveMeReports',device);
err=PsychHID('SetReport',device,2,68,uint8(68)); % GetStatus
if err.n
    fprintf('GetStatus SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
if 0
    [report,err]=PsychHID('GetReport',device,1,68,3); % GetStatus
    if err.n
        fprintf('DaqGetStatus GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
    end
else
    err=PsychHID('ReceiveReports',device);
    if err.n
        fprintf('DaqGetStatus ReceiveReports error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
    end
    [reports,err]=PsychHID('GiveMeReports',device);
    if err.n
        fprintf('DaqGetStatus GiveMeReports error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
    end
    report=[];
    for i=1:length(reports)
        if reports(i).report(1)==68
            report=reports(1).report;
        end
    end
end
if length(report)==3 && report(1)==68
    % 16 bits, only 3 of which are assigned.
    status.master=bitget(report(2),1); % 0 sync slave; 1 sync master.
    status.rising=bitget(report(2),2); % 0 trigger on falling edge; 1 trigger on rising edge
    status.program=bitget(report(3),8); % 1 program memory update mode.
else
    status=[];
end
err=PsychHID('ReceiveReportsStop',device);

