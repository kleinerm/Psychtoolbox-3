function params=DaqAInScanContinue(daq,options)
% params=DaqAInScanContinue(DeviceIndex,options)
% Calls DaqAInScan with the options set to only continue, and not begin or
% end. You may call DaqAInScanContinue as many times as you like, to keep
% transferring reports from Mac OS to PsychHID. Eventually you should 
% call DaqAInScanEnd to get the data. 
% See also DaqAInScan, DaqAInScanBegin, DaqAInScanEnd,
% Daq, DaqPins, DaqTest, PsychHIDTest.

% 4/15/05 dgp Wrote it.
% 1/10/08 mpr glanced at it ... added return statement per TheMathworks'
%                 suggestion, changed TestDaq and TestPsychHid to
%                 DaqTest and PsychHIDTest in comments above, removed
%                 self-reference in "See also" list, changed variable
%                 "device" to "daq" in effort to bring consistency across
%                 functions.

options.begin=0;
options.continue=1;
options.end=0;
[data,params]=DaqAInScan(daq,options);

return;
