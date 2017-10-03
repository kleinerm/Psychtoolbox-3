function info = GetTouchDeviceInfo(deviceIndex)
% info = GetTouchDeviceInfo(deviceIndex)
%
% Return 'info' a struct with info about the specified touch input device
% 'deviceIndex'.
%
% WINDOWS: ________________________________________________________________
%
% This function currently returns nothing, as touch devices aren't yet
% supported.
%
% OS X: ___________________________________________________________________
%
% This function currently returns nothing, as OSX does not support touch
% screens in a meaningful way as far as we know. And Psychtoolbox for OSX
% currently does not implement any special support for touchpads or such.
%
% _________________________________________________________________________________
% see also: GetTouchDeviceIndices

% HISTORY
% 03-Oct-2017 mk  Wrote it.

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
    [~, ~, ~, ~, ~, info.valuatorInfos] = GetMouse([], deviceIndex);
else
    info.valuatorInfos = [];
end

return;
