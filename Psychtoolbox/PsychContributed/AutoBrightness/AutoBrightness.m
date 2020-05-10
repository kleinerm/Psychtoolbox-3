function [oldSetting, status] = AutoBrightness(screenNumber, newSetting)
% [oldSetting, status] = AutoBrightness([screenNumber=0][, newSetting])
%
% OBSOLETE: Use MacDisplaySettings instead. It's faster and more reliable.
% To support old programs we have rewritten this to simply call
% MacDisplaySettings. However, it's probably best to replace your call to
% AutoBrightness by your own call to MacDisplaySettings, using what's below
% as a model. If you need AutoBrightness you probably also need more of the
% controls offered by MacDisplaySettings.
%
% AUTOBRIGHTNESS. Get and set the "Automatically adjust brightness"
% checkbox on the Mac OS X: System Preferences: Displays panel. The
% function argument "newSetting" (integer 0 or 1) indicates whether you
% want to turn the autobrightness feature on (newSetting==1) or off
% (newSetting==0). If you call without an argument (or anything other than
% 0 or 1) then nothing is changed. The current state is always reported in
% the returned oldSetting (0 or 1). The optionally returned "status" is
% always zero unless the applescript failed.
%
% HISTORY: Written by denis.pelli@nyu.edu for the Psychtoolbox, May 21,
% 2015. Incorporated into MATLAB adding untested code to specify which
% screen, by Mario Kleiner, June 1. Convert return argument from string to
% double, by Denis, June 11, 2015. Replaced by a call to the new
% MacDisplaySettings (as suggested by Mario) Denis Pelli, May 6, 2020.
%
% See also:
% MacDisplaySettings.m
% May 6, 2020, denis.pelli@nyu.edu

if ~IsOSX
    oldSetting = 0;
    status = 1; % Signal failure on this unsupported OS.
    return
end
if nargin<1
    screenNumber=0;
end
if nargin<2
    newSetting=[];
end
newSettings.automatically=newSetting;
[oldSettings,errorMessage]=MacDisplaySettings(screenNumber,newSettings);
oldSetting=oldSettings.automatically;
if ~isempty(errorMessage)
    warning('%s',errorMessage);
end
status=~isempty(errorMessage);
end
