daq = DaqFind;
reportId = 3;
TheReport = uint8(0);
NumberOfPorts = 2;
options.secs = 0.000;
PsychHID('ReceiveReportsStop',daq);
PsychHID('GiveMeReports',daq);
PsychHID('ReceiveReports',daq, options);

while 1
% Emit query to device:
PsychHID('SetReport',daq,2,reportId, TheReport);

% Wait for result from device:
inreport = [];
while isempty(inreport)
% Yield some minimum amount of time to other processes if you want to be
WaitSecs('YieldSecs', 0);
% New data available?
inreport = PsychHID('GetReport',daq,1,reportId,NumberOfPorts+1);
end

% inreport contains latest button query result: Button pressed?
if inreport(3) ~= 255
% yes: Done!
break;
end

% No. Repeat sampling...
end