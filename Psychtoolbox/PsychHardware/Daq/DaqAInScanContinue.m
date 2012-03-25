function [params, data] = DaqAInScanContinue(daq,options,wantLiveData)
% [params, data] = DaqAInScanContinue(DeviceIndex, options [, wantLiveData=0])
%
% Calls DaqAInScan with the options set to only continue, and not begin or
% end. You may call DaqAInScanContinue as many times as you like, to keep
% transferring reports from the system to PsychHID. Eventually you should 
% call DaqAInScanEnd to end aquisition and get all remaining data.
%
% If you want to retrieve already captured data, set the optional
% 'wantLiveData' flag to 1. This will return all already received data in
% the optional return argument 'data', in the same format as DaqAInScanEnd
% would provide.
%
% See also DaqAInScan, DaqAInScanBegin, DaqAInScanEnd,
% Daq, DaqPins, DaqTest, PsychHIDTest.

% 4/15/05 dgp Wrote it.
% 1/10/08 mpr glanced at it ... added return statement per TheMathworks'
%                 suggestion, changed TestDaq and TestPsychHid to
%                 DaqTest and PsychHIDTest in comments above, removed
%                 self-reference in "See also" list, changed variable
%                 "device" to "daq" in effort to bring consistency across
%                 functions.
% 3/24/12 mk  Add optional 'wantLiveData' flag to retrieve data while still
%             capturing.

options.begin=0;
options.continue=1;
options.end=0;

% Optional flag wantLiveData set to 1?
if (nargin >= 3) && wantLiveData
    % Return currently available data:
    options.livedata = 1;
else
    % Only drive the scan, don't return currently buffered data:
    options.livedata = 0;
end

[data, params]=DaqAInScan(daq,options);

return;
