function params=DaqAInScanContinue(device,options)
% params=DaqAInScanContinue(device,options)
% Calls DaqAInScan with the options set to only continue, and not begin or
% end. You may call DaqAInScanContinue as many times as you like, to keep
% transferring reports from Mac OS to PsychHID. Eventually you should 
% call DaqAInScanEnd to get the data. 
% See also DaqAInScan, DaqAInScanBegin, DaqAInScanContinue, DaqAInScanEnd,
% Daq, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

options.begin=0;
options.continue=1;
options.end=0;
[data,params]=DaqAInScan(device,options);

