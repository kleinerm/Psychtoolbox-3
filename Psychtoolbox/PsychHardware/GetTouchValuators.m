function event = GetTouchValuators(event, deviceInfo)
% event = GetTouchValuators(event, deviceInfo)
%
% Return 'event', an augmented version of the input 'event', ie. event extended
% with info about additional semantic meaning of the specified touch input event,
% using device specific semantic mapping info provided in 'deviceInfo'.
%
% Typical use:
%
% 1. Get 'deviceInfo' at script startup:
%    deviceInfo = GetTouchDeviceInfo(deviceIndex);
%
% 2. For each retrieved touch event evt = TouchEventGet(...):
%    evt = GetTouchValuators(evt, deviceInfo)
%
% Which information is available on a given operating system/display system and
% device combo is variable, but the returned event struct potentially has the
% following additional fields:
%
% .RawX = Raw device x position of touch.
%
% .RawY = Raw device y position of touch.
%
% .ToolX = Raw device x position of approaching tool, if detectable.
%
% .ToolY = Raw device y position of approaching tool, if detectable.
%
% .ToolType = Type of tool used on the surface. There are numbers for finger,
%             palm, and different type of styluses.
%
% .Pressure = Pressure which the touch applies to the touch device surface,
%             normalized to 0.0 - 1.0 range.
%
% .Distance = Distance from the touch surface, normalized to 0.0 - 1.0 range,
%             with 0.0 = contact, 1.0 = Maximally far away while still being
%             detectable, iow. hovering over the surface.
%
% .TouchMajor = Length of major axis of an ellipse which approximates the shape
%               of the touch area, in device units.
%
% .TouchMinor = Length of minor axis of an ellipse which approximates the shape
%               of the touch area, in device units.
%
% .WidthMajor and .WidthMinor are major/minor width of an ellipse approximating
%  the complete object approaching the screen. The ratio TouchMajor / WidthMajor
%  and TouchMinor / WidthMinor can be used to approximate how close/strong a touch
%  is, in the absence of .Pressure or .Distance information.
%
% .Orientation = If the touch contact area isn't circular and the hardware can
%                detect the orientation of a touch area, then this stores orientation
%                in degrees. 0 degrees = Upward pointing - aligned with positive
%                y-axis of the touch surface. > 0 degress = clock-wise turn, < 0
%                degrees = counter clock-wise turn. Many devices can not find out
%                orientation at all, or only if a contact is more vertical (0 deg.)
%                rather more horizontal (90 deg.).

% ______________________________________________________________________________
% see also: GetTouchDeviceIndices, GetTouchDeviceInfo, TouchEventGet

% HISTORY
% 03-Oct-2017 mk  Wrote it.

if nargin < 1 || isempty(event) || ~ismember(event.Type, [2,3,4])
    error('Required touch event missing, or event is not a standard touch event.');
end

if nargin < 2 || isempty(deviceInfo)
    error('Required deviceInfo missing.');
end

% Extract what we need atm.:
valuators = event.Valuators;
valuatorInfos = deviceInfo.valuatorInfos;

for i=1:length(valuatorInfos)
    label = valuatorInfos(i).label;
    if strcmpi(label, 'Abs MT Position X')
        event.RawX = valuators(i);
    end

    if strcmpi(label, 'Abs MT Position Y')
        event.RawY = valuators(i);
    end

    if strcmpi(label, 'Abs MT Pressure')
        event.Pressure = valuators(i) / valuatorInfos(i).max;
    end

    if strcmpi(label, 'Abs MT Distance')
        event.Distance = valuators(i) / valuatorInfos(i).max;
    end

    if strcmpi(label, 'Abs MT Touch Major')
        event.TouchMajor = valuators(i);
    end

    if strcmpi(label, 'Abs MT Touch Minor')
        event.TouchMinor = valuators(i);
    end

    if strcmpi(label, 'Abs MT Width Major')
        event.WidthMajor = valuators(i);
    end

    if strcmpi(label, 'Abs MT Width Minor')
        event.WidthMinor = valuators(i);
    end

    if strcmpi(label, 'Abs MT Orientation')
        % Signed value, where .max corresponds to 90 degrees clock-wise:
        event.Orientation = valuators(i) / valuatorInfos(i).max * 90;
    end

    if strcmpi(label, 'Abs MT Tool X')
        event.ToolX = valuators(i);
    end

    if strcmpi(label, 'Abs MT Tool Y')
        event.ToolY = valuators(i);
    end

    if strcmpi(label, 'Abs MT Tool Type')
        event.ToolType = valuators(i);
    end
end
