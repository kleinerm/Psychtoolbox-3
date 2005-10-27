function err=DaqALoadQueue(device,channel,range)
% err=DaqALoadQueue(device,channel,range)
% USB-1208FS: Load the channel/gain queue. The USB-1208FS can scan an
% arbitrary sequence of analog input channels, each with an arbitrary gain
% setting. DaqALoadQueue allows you to specify that sequence. 
% "device" is a small integer, the array index specifying which HID
%     device in the array returned by PsychHID('Devices') is interface 0
%     of the desired USB-1208FS box.
% "channel" is a vector of length 1 to 8; each value (0 to 15) selects any of
%     various single-ended or differential measurements.
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
% "range" is a vector of the same length, with values of 0 to 7, specifying
%     the desired gain (and voltage range) for the corresponding channel.
%     0 for 1x (+-20 V),  1 for 2x (+-20 V),
%     2 for 4x (+-20 V),  3 for 5x (+-20 V),
%     4 for 8x (+-20 V),  5 for 10x (+-20 V),
%     6 for 16x (+-20 V), 7 for 20x (+-1 V).
% See also Daq, DaqFunctions, DaqPins, TestDaq, TestPsychHid, DaqAIn, DaqAInScan, DaqAInScanBegin.

% 4/15/05 dgp Wrote it.

if length(channel)~=length(range)
    error('Length of "channel" and "range" vectors must be equal.');
end
if length(channel)~=numel(channel) || length(range)~=numel(range)
    error('"channel" and "range" must be vectors.');
end
if any(~ismember(channel,0:15)) || any(~ismember(range,0:7))
    error('Each "channel" value must be in 0:15 and each range value must be in 0:7.');
end
report=zeros(1,2+2*length(channel));
report(1)=19;
report(2)=length(channel);
for i=1:length(channel)
    report(2*i+1)=channel(i);
    report(2*i+2)=range(i);
end
err=PsychHID('SetReport',device,2,19,uint8(report)); % ALoadQueue
if err.n
    fprintf('DaqALoadQueue error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end

