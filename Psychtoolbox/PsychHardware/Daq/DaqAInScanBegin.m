function params=DaqAInScanBegin(device,options)
% params=DaqAInScanBegin(device,options)
% Calls DaqAInScan with the options set to only begin, and not continue or
% end. You should subsequently call DaqAInScanContinue (as many times as
% you like) and, finally, DaqAInScanEnd to get the data. For example:
%     options.lowChannel=lowChannel;
%     options.highChannel=highChannel;
%     options.count=count;
%     options.f=f;
%     params=DaqAInScanBegin(device,options);
%     for i=1:frames
%         % add code here doing something useful
%         params=DaqAInScanContinue(device,options);
%     end
%     params=DaqAInScanContinue(device,options);
%     [data,params]=DaqAInScanEnd(device,options);
% See also DaqAInScan, DaqAInScanBegin, DaqAInScanContinue, DaqAInScanEnd,
% Daq, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

options.begin=1;
options.continue=0;
options.end=0;
[data,params]=DaqAInScan(device,options);

