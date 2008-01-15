function v=DaqAIn(daq,channel,range,UnCal)
% v=DaqAIn(DeviceIndex,channel,range,[DoNotCalibrate])
% Analog input.
% USB-1208FS:
% "v" is the measured voltage, in Volts, or NaN if no data were received.
% "DeviceIndex" is a small integer, the array index specifying which HID
%     device in the array returned by PsychHID('Devices') is interface 0
%     of the desired Daq device.
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
%     0 for Gain 1x (+-20 V),   1 for Gain 2x (+-10 V),
%     2 for Gain 4x (+-5 V),    3 for Gain 5x (+-4 V),
%     4 for Gain 8x (+-2.5 V),  5 for Gain 10x (+-2 V),
%     6 for Gain 16x (+-1.25 V),  7 for Gain 20x (+-1 V).
% "DoNotCalibrate" for 1208FS has no effect
%
% USB-1608FS:
% only single-ended inputs are defined, so "channel" is restricted to integers
% from 0 to 7.  Ignore the manual which claims that gains are restricted to only
% four possible values.  Thank the linux people for discovering:
%      0 for Gain 1x (+/- 10 V),      1 for Gain 2x (+/-5 V),
%      2 for Gain 4x (+/- 2.5 V),     3 for Gain 5x (+/- 2 V),
%      4 for Gain 8x (+/- 1.25 V),    5 for Gain 10x (+/- 1V),
%      6 for Gain 16x (+/- 0.625 V),  7 for Gain 32x (+/- 0.3125 V)
%
% "DoNotCalibrate" (0 or 1) if non-zero, function does not use information
% acquired from calibration measurements (see DaqCalibrateAIn).  This should
% probably always be 0 when function is called by a user (hence it defaults to
% 0).  Flag was created for the purpose of acquiring calibration data.
% Otherwise subsequent calibration measurements would be fits of fits.
%
% See also Daq, DaqFunctions, DaqPins, DaqTest, PsychHIDTest,
% DaqDeviceIndex, DaqDIn, DaqDOut, DaqAIn, DaqAOut, DaqAInScan,DaqAOutScan.
%
% 4/15/05 dgp Wrote it.
% 1/21/07 asg changed normalization value from 2^16 to 2^15 to account for 16th bit as a sign bit (not data bit)
%             and modified the "range" value help.
% 6/17/07 mk  Add proper sign handling for negative voltages.
% 12/2x/07-1/x/08  mpr   modified to work with USB-1608FS and changed some
%                           terminology; particularly did away with "sign" name
%                           conflict ("sign" is a Matlab function)

devices = PsychHID('Devices');

if nargin < 4 || isempty(UnCal)
  UnCal=0;
end

if strcmp(devices(daq).product(5:6),'16')
  Is1608=1;
  MaxChannelID = 7;
else
  Is1608=0;
  MaxChannelID = 15;
end

err1=PsychHID('ReceiveReports',daq);
err2=PsychHID('ReceiveReportsStop',daq);
[reports,err]=PsychHID('GiveMeReports',daq);
if ~ismember(channel,0:MaxChannelID)
    error(sprintf('DaqAIn: "channel" must be an integer 0 to %d.',MaxChannelID));
end
if ~ismember(range,0:7)
    error('DaqAIn: "range" must be an integer 0 to 7.');
end
err3=PsychHID('SetReport',daq,2,16,uint8([16 channel range])); % Read analog input
if err.n
    fprintf('DaqAIn SetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
err4=PsychHID('ReceiveReports',daq);
[report,err5]=PsychHID('GetReport',daq,1,16,3); % Get report

if err.n
    fprintf('DaqAIn GetReport error 0x%s. %s: %s\n',hexstr(err.n),err.name,err.description);
end
err6=PsychHID('ReceiveReportsStop',daq);

if length(report)==3
  if Is1608
    vmax = [10 5 2.5 2 1.25 1 0.625 0.3125];

    RawValue = double(report(3))*256 + double(report(2));
    v = vmax(range+1)*(RawValue/32768-1);
    if UnCal
      return; 
    elseif exist('~/Library/Preferences/PsychToolbox/DaqToolbox/DaqPrefs.mat','file');
      DaqVars = load('~/Library/Preferences/PsychToolbox/DaqToolbox/DaqPrefs');
      if isfield(DaqVars,'CalData')
        CalData = DaqVars.CalData;
        GoodIndices = find(CalData(:,1) == channel & CalData(:,2) == range);
        if ~isempty(GoodIndices)
          TheDays = CalData(GoodIndices,end);
          ThisDay = datenum(date);
          [DaysSinceLast,BestIndex] = min(ThisDay-TheDays);
          AllThatDay = find(TheDays == TheDays(BestIndex));
          MostRecentPolyFit = CalData(GoodIndices(AllThatDay(end)),3:5);
          
          if DaysSinceLast > 30
            warning('Calibration of this channel has not been performed since %s!!',datestr(MostRecentData(1,end)));
          end
          
          v = polyval(MostRecentPolyFit,v);
          return;
        end
      end
    end
      
    warning('It looks like this channel has not yet been calibrated.  In my tests, uncalibrated values could be off by as much as 15%!');
  else
    % I'm really not sure that calibration is handled properly here because
    % the manual indicates that for single ended measurements, the range is
    % always +/- 10 V.  For differential inputs it seems correct, though. --
    % mpr

    % Mapping table value -> voltage for different gains:
    vmax=[20,10,5,4,2.5,2,1.25,1];

    % Extract and strip sign bit from high-byte; near as I can tell, this code
    % was quite broken, so I fixed it, but I don't have a 1208FS device and
    % hence can't test it... -- mpr
    TheSign = -2*double(bitget(report(3), 8))+1;

    report(3) = bitand(report(3), 1+2+4+8+16+32+64);
    report=double(report);
    
    if UnCal
      v = TheSign*(report(2)+report(3)*256);
    else
      % Reassemble low-byte, high-byte and sign into signed voltage level:
      v=TheSign * vmax(range+1)*(report(2)+report(3)*256)/32768;    % added/changed by asg due to advice from Mario (forum post #5713)
    end
  end    
else
%   fprintf('length(report) %d\n',length(report));
    v=NaN;
end

return;


