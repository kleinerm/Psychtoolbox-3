function [data,params]=DaqAInScanEnd(device,options)
% [data,params]=DaqAInScanEnd(device,options)
% Calls DaqAInScan with the options set to just end the scan, returning the
% data to you. 
% See also DaqAInScan, DaqAInScanBegin, DaqAInScanContinue, DaqAInScanEnd,
% Daq, DaqPins, TestDaq, TestPsychHid.

% 4/15/05 dgp Wrote it.

options.begin=0;
options.continue=0;
options.end=1;
[data,params]=DaqAInScan(device,options);

