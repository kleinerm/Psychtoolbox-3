function [data,params]=DaqAInScanEnd(daq,options)
% [data,params]=DaqAInScanEnd(DeviceIndex,options)
% Calls DaqAInScan with the options set to just end the scan, returning the
% data to you. 
% See also DaqAInScan, DaqAInScanBegin, DaqAInScanContinue, 
% Daq, DaqPins, DaqTest, PsychHIDTest.

% 4/15/05 dgp Wrote it.
% 1/10/08 mpr glanced at it (made same changes as in DaqAInScanContinue)

options.begin=0;
options.continue=0;
options.end=1;
[data,params]=DaqAInScan(daq,options);

return;
