function info =GetTouchValuators(valuators, valuatorInfos)
% info = GetTouchValuators(valuators, valuatorInfos)
%
% Return 'info' a struct with info about the specified touch input device
% 'valuators', using mapping info 'valuatorInfos'.
%
% Typical use:
%
% 1. Get 'valuatorinfos' at script startup:
%    info = GetTouchDeviceInfo(deviceIndex);
%    valuatorInfos = info.valuatorInfos;
%
% 2. For each retrieved touch event evt = TouchEventGet(...):
%    info = GetTouchValuators(evt.Valuators, valuatorInfos)
%
% The returned info struct potentiallyy has the following fields:
% TODO XXX
% ______________________________________________________________________________
% see also: GetTouchDeviceIndices, GetTouchDeviceInfo, TouchEventGet

% HISTORY
% 03-Oct-2017 mk  Wrote it.

if nargin < 1 || isempty(valuators)
    error('Required valuators missing.');
end

if nargin < 2 || isempty(valuatorInfos)
    error('Required valuatorInfos missing.');
end

for i=1:length(valuatorInfos)
    label = valuatorInfos(i).label;
    if strcmpi(label, 'Abs MT Position X')
        info.X = valuators(i);
    end

    if strcmpi(label, 'Abs MT Position Y')
        info.Y = valuators(i);
    end

    if strcmpi(label, 'Abs MT Touch Pressure')
        info.Pressure = valuators(i) / valuatorInfos(i).max;
    end

    if strcmpi(label, 'Abs MT Touch Distance')
        info.Distance = valuators(i) / valuatorInfos(i).max;
    end

    if strcmpi(label, 'Abs MT Touch Major')
        info.TouchMajor = valuators(i);
    end

    if strcmpi(label, 'Abs MT Touch Minor')
        info.TouchMinor = valuators(i);
    end

    if strcmpi(label, 'Abs MT Width Major')
        info.WidthMajor = valuators(i);
    end

    if strcmpi(label, 'Abs MT Width Minor')
        info.WidthMinor = valuators(i);
    end

    if strcmpi(label, 'Abs MT Orientation')
        % Signed value, where .max corresponds to 90 degrees clock-wise:
        info.Orientation = valuators(i) / valuatorInfos(i).max * 90;
    end
end
