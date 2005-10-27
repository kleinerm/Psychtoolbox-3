function data=DaqGetAll(device)
% data=DaqGetAll(device)
% USB-1208FS: Retrieve all analog and digital input values. This command
% reads the value from all analog input channels and digital I/Os.
% "device" is a small integer, the array index specifying which HID
%       device in the array returned by PsychHID('Devices') is interface 0 
%       of the desired USB-1208FS box.
% data.analog is a 1x16 array of the values read from the analog input
%       channels.
% data.digital(1) is the value (0 to 255) read from digital port A.
% data.digital(2) is the value (0 to 255) read from digital port B.
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

err=PsychHID('ReceiveReports',device);
err=PsychHID('ReceiveReportsStop',device);
PsychHID('GiveMeReports',device);
err=PsychHID('SetReport',device,2,70,uint8(70)); % GetAll
if err.n
    fprintf('GetAll SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
if 0
    [report,err]=PsychHID('GetReport',device,1,70,35);
    if err.n
        fprintf('GetAll GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
        data=[];
    end
else
    err=PsychHID('ReceiveReports',device);
    [reports,err]=PsychHID('GiveMeReports',device);
    if err.n
        fprintf('GetAll GiveMeReports error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
    end
    report=[];
    for i=1:length(reports)
        if reports(i).report(1)==70
            report=reports(1).report;
        end
    end
end
if ~isempty(report) && report(1)==70
    % 35 byte report
    data.analog=zeros(1,16);
    for i=1:16
        data.analog(i)=report(i*2)+256*report(i*2+1);
        % if sign bit is set, make it negative
        if data.analog(i)>(2^16-1)
            data.analog(i)=data.analog(i)-2^16;
        end
    end
    data.digital=double(report(34:35));
else
    data=[];
end
err=PsychHID('ReceiveReportsStop',device);
return

