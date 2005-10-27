function v=DaqAIn(device,channel,range)
% v=DaqAIn(device,channel,range)
% USB-1208FS analog input.
% "v" is the measured voltage, in Volts, or NaN if no data were received.
% "device" is a small integer, the array index specifying which HID
%     device in the array returned by PsychHID('Devices') is interface 0
%     of the desired USB-1208FS box.
% "channel" (0 to 15) selects any of various single-ended or differential 
%     measurements.
%     "channel" Measurement
%      0        0-1 (differential)
%      1        2-3 (differential)
%      2        4-5 (differential)
%      3        6-7 (differential)
%      4        1-0 (differential)
%      5        3-2 (differential)
%      6        5-4 (differential)
%      7        7-6 (differential)
%      8          0 (single-ended)
%      9          1 (single-ended)
%     10          2 (single-ended)
%     11          3 (single-ended)
%     12          4 (single-ended)
%     13          5 (single-ended)
%     14          6 (single-ended)
%     15          7 (single-ended)
% "range" (0 to 7) sets the gain (and voltage range):
%     0 for 1x (+-20 V),  1 for 2x (+-20 V),
%     2 for 4x (+-20 V),  3 for 5x (+-20 V),
%     4 for 8x (+-20 V),  5 for 10x (+-20 V),
%     6 for 16x (+-20 V), 7 for 20x (+-1 V).
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid,
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.

% 4/15/05 dgp Wrote it.

err=PsychHID('ReceiveReports',device);
err=PsychHID('ReceiveReportsStop',device);
[reports,err]=PsychHID('GiveMeReports',device);
if ~ismember(channel,0:15)
    error('DaqAIn: "channel" must be an integer 0 to 15.');
end
if ~ismember(range,0:7)
    error('DaqAIn: "range" must be an integer 0 to 7.');
end
err=PsychHID('SetReport',device,2,16,uint8([16 channel range])); % Read analog input
if err.n
    fprintf('DaqAIn SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
err=PsychHID('ReceiveReports',device);
[report,err]=PsychHID('GetReport',device,1,16,3); % Get report
if err.n
    fprintf('DaqAIn GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
if length(report)==3
    vmax=[20,10,5,4,2.5,2,1.25,1];
    report=double(report);
    v=vmax(range+1)*(report(2)+report(3)*256)/65535;
else
%     fprintf('length(report) %d\n',length(report));
    v=nan;
end
err=PsychHID('ReceiveReportsStop',device);

