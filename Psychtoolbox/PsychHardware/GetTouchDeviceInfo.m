function info = GetTouchDeviceInfo(deviceIndex)
% info = GetTouchDeviceInfo(deviceIndex)
%
% Return 'info' a struct with info about the specified touch input device
% 'deviceIndex'. 'info' has the following fields:
%
% 'product', 'serialNumber' and 'locationID' are self-explanatory.
%
% 'maxTouchpoints' The maximum number of simultaneously supported touch
% points on the device. 10 is a quite common number for typical touchscreens.
%
% 'touchDeviceType': 0 = Touchpad, 1 = Touchscreen.
%
% 'valuatorInfos': A struct array. Each slot contains info about the meaning,
% parameter range and resolution of the corresponding value in the Valuators
% vector returned as part of touch events. i'th slot == i'th vector entry.
% This is mostly for use with GetTouchValuators() to map raw valuator values
% into something meaningful.
%
% OS X: ___________________________________________________________________________
%
% This function currently returns nothing, as macOS does not support touch
% screens in a meaningful way as far as we know. And Psychtoolbox for macOS
% currently does not implement any special support for touchpads or such.
%
% _________________________________________________________________________________
% see also: GetTouchDeviceIndices, GetTouchValuators, TouchEventGet

% HISTORY
% 03-Oct-2017 mk  Wrote it.
% 02-Feb-2021 mk  Add what would be returned by a Windows-10 system, if we'd support it.

touchIndices=[];
productNames=cell(0);
allInfo=cell(0);

if nargin < 1 || isempty(deviceIndex)
    error('Required deviceIndex missing.');
end

if ~ismember(deviceIndex, GetTouchDeviceIndices)
    error('Provided deviceIndex is not a touch input device.');
end

% Get info from PsychHID Devices:
[idx, ~, allinfos] = GetTouchDeviceIndices;
i = find(idx == deviceIndex);
allinfos = allinfos{i};

info.touchDeviceType = allinfos.touchDeviceType;
info.maxTouchpoints = allinfos.maxTouchpoints;
info.product = allinfos.product;
info.serialNumber = allinfos.serialNumber;
info.locationID = allinfos.locationID;

% Get info about the different valuators:
if IsLinux
    % Query dynamic list from Linux. Will change depending on OS version and
    % specific capabilities of the digitizer device:
    [~, ~, ~, ~, ~, info.valuatorInfos] = GetMouse([], deviceIndex);
else
    if IsWin
        % Windows touch input, as of Windows-10 early February 2021, provides only
        % the following hard-coded properties (at most). See following for reference:
        % https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-touchinput

        % Always: X and Y 2D touch position, redundantly exposed as valuator:
        info.valuatorInfos(1).label = 'Abs MT Position X';
        info.valuatorInfos(2).label = 'Abs MT Position Y';

        % Minor/Major (width/height of approximated touch bounding box) are
        % optional, and dependent on digitizer device:
        info.valuatorInfos(3).label = 'Abs MT Touch Minor';
        info.valuatorInfos(4).label = 'Abs MT Touch Major';

        % There's one more optional (dependent on digitizer device) scalar
        % field, whose meaning is unspecified. Could be pressure, proximity, or
        % whatever. We'd return realmin ('single') if the field is not supported:
        info.valuatorInfos(5).label = 'ExtraInfo';
    else
        info.valuatorInfos = [];
    end
end

return;
