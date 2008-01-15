function DaqCalibrateAIn(daq,channel)
% Syntax: DaqCalibrateAIn(DeviceID,AnalogChannel)
%
% Purpose: Measure output of calibration pin to provide calibration of the
%           input "AnalogChannel" on the device "DeviceID".
%
% History:  1/10/08   mpr   decided to calibrate good times come on!
%
% This function was written for the USB-1608FS.  It could be easily modified for
% a 1208FS, but since there are only two levels of the output on that device it
% seems like it would be a bit less useful.  If you want to modify this for such
% a device, please feel free.  Since I don't have one, I'm not motivated... 
% If you have a 1608FS, this function walks through the outputs of the
% calibration terminal and the gain settings of the analog channel and produces
% corrections that will affect the interpretation of numbers derived from other
% functions (e.g., DaqAIn and DaqAInScan).  "AnalogChannel" can be a vector
% (values range from 0:7) if you want to calibrate multiple channels at once.
% Default channel is 0, and if no argument is passed for DeviceID, DaqFind will
% be run to see if you have only one device.  -- mpr

if nargin < 2 || isempty(channel)
  channel = 0;
end

if nargin < 1 || isempty(daq)
  daq=DaqFind;
end

% Flag to get DaqAIn to return raw digital responses
UnCal=1;

AllHIDDevices = PsychHID('Devices');
if isempty(strfind(AllHIDDevices(daq).product,'1608'))
  error(sprintf(['It appears that you are not using a USB-1608FS.  If that is correct, then you \n' ...
                 'should modify DaqCalibrateAIn if you want to use the function for your device.\n']));
end

% in case user does something bizarre and passes the same value more than once
channel = unique(channel);

if any(~ismember(channel,0:7))
  error('Channels should range from 0 through 7, and it looks like yours don''t.');
end

CalVs = [0 0.625 1.25 2.5 5];
VMaxs = [10 5 2.5 2 1.25 1 0.625 0.3125];

PrefsExist = exist('~/Library/Preferences/PsychToolbox/DaqToolbox/DaqPrefs.mat','file');
if PrefsExist
  DaqVars=load('~/Library/Preferences/PsychToolbox/DaqToolbox/DaqPrefs');
  OldGains = DaqVars.OldGains;
else
  if ~exist('~/Library/Preferences/PsychToolbox','dir')
    [DirMade, DirMessage] = mkdir('~/Library/Preferences','PsychToolbox');
    if DirMade
      if ~exist('~/Library/Preferences/PsychToolbox/DaqToolbox','dir')
        [DirMade, DirMessage] = mkdir('~/Library/Preferences/PsychToolbox','DaqToolbox');
      end
    end % if DirMade
  end % if ~exist('~/Library/Preferences/PsychToolbox','dir')
end % if PrefsExist; else

if ~PrefsExist && ~DirMade
  error('I could not find a preferences file, and I could not create the directory for where I should put one...');
end

if PrefsExist
  DaqVars = load('~/Library/Preferences/PsychToolbox/DaqToolbox/DaqPrefs');
  if isfield(DaqVars,'CalData')
    CalData = DaqVars.CalData;
  else
    CalData = [];
    PrefsExist=0;
    DirMade=1;
  end
end

for k=1:length(channel)
  for l=1:length(VMaxs)
    VIns = [];
    DOuts = [];
    for m=1:length(CalVs)
      if CalVs(m) <= VMaxs(l)
        DaqSetCal(daq,m-1);
        VIns(end+1) = CalVs(m);
        for n=1:5
          TmpD(n) = DaqAIn(daq,channel(k),l-1,UnCal);
        end
        DOuts(end+1) = mean(TmpD);
      else
        break;
      end
    end
    if length(VIns) > 2
      CalData(end+1,:) = [channel(k) l-1 polyfit(DOuts,VIns,2) datenum(date)];
    elseif length(VIns) > 1
      CalData(end+1,:) = [channel(k) l-1 0 polyfit(DOuts,VIns,1) datenum(date)];
    else
      CalData(end+1,:) = [channel(k) l-1 0 1 -DOuts datenum(date)];
    end
  end
end

DaqVars.CalData = CalData;

save('~/Library/Preferences/PsychToolbox/DaqToolbox/DaqPrefs','-Struct','DaqVars');

return;
