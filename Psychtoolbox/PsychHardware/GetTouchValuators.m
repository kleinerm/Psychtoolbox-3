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
% Which valuators are available on a given operating system + device combo
% is variable, but the returned info struct potentially has the following fields:
%
% .X = Raw device x position of touch.
%
% .Y = Raw device y position of touch.
%
% .ToolX = Raw device x position of approaching tool, if detectable.
%
% .ToolY = Raw device y position of approaching tool, if detectable.
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
%  and TouchMinor / WidthMinor can be used to approximate how close/trong a touch
%  in the absence of .Pressure or .Distance information.
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

    if strcmpi(label, 'Abs MT Tool X')
        info.ToolX = valuators(i);
    end

    if strcmpi(label, 'Abs MT Tool Y')
        info.ToolY = valuators(i);
    end
end
