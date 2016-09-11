function [gamepadIndices, productNames, allInfos]= GetGamepadIndices(product, serialNumber, locationID)
% [gamepadIndices, productNames, allInfos] = GetGamepadIndices([productName][, serialNumber][, locationID])
%
% PsychHID assigns each USB HID device connected to you computer a
% unique index. GetGamepadIndices returns the indices for those HID devices
% which are gamepads. The product names of each gamepad are returned in a
% second argument which is useful to identify the gamepad associated with
% an index. The third return argument is a cell-array with complete
% information about the gamepad device, e.g., allInfos{1} returns
% all known info about the 1st detected gamepad.
%
% If you have multiple gamepads connected you can restrict the set of
% returned gamepads by specifying the following optional match-critera:
%
% product      = Product name of target devices, as returned in 'productNames'.
%
% serialNumber = Serial number of target devices. This is a text string,
%                not a number!
%
% locationID   = Numeric id of where the device is connected to the
%                computer. The number is supposed to be unique for a given
%                connection port. E.g., the same number will be returned
%                whenever the device is connected to the same USB port of
%                the computer. The value should be persistent across
%                reboots of the machine, but may not be persistent across
%                operating system upgrades - or may not be persistent at
%                all in case of os bugs. Your mileage may vary...
%                On Linux X11 this is currently remapped to interfaceID,
%                as locationID is not meaningful in the current implementation.
%
% WINDOWS: ________________________________________________________________
% 
% GetGamepadIndices does not work on Windows.
% 
% _________________________________________________________________________
%
% see also: GetKeyboardIndices

% HISTORY
% 7/6/03    awi     Wrote it.
% 7/13/04   awi     Improved documentation
% 1/20/05   mk      Added fix for Logitech joystick (Forum msg. 4149)
% 9/07/16   mk      Add matching logic for selection of subsets of devices.

if nargin < 1
    product = [];
end

if nargin < 2
    serialNumber = [];
end

if nargin < 3
    locationID = [];
end

gamepadIndices=[];
productNames=cell(0);
allInfos=cell(0);

if ~IsOSX
  % On Linux, a mouse is a gamepad, is a mouse...
  d = [ PsychHID('Devices', 3) , PsychHID('Devices', 5) ];
else
  d=PsychHID('Devices');
end

for i = 1:length(d)
    if IsLinux || (d(i).usagePageValue==1 && (d(i).usageValue == 5 || d(i).usageValue == 4))
        % Check if additional match-criteria provided. Skip this device on mismatch:
        if ~isempty(product) && ~strcmpi(d(i).product, product)
            continue;
        end

        if ~isempty(serialNumber) && ~strcmpi(d(i).serialNumber, serialNumber)
            continue;
        end

        if ~isempty(locationID) && IsOSX && (d(i).locationID ~= locationID)
            continue;
        end

        if ~isempty(locationID) && IsLinux && (d(i).interfaceID ~= locationID)
            continue;
        end

        % Match! Append to list of gamepads, including optional
        % detailed information:
        gamepadIndices(end+1)=d(i).index; %#ok<AGROW>
        productNames{end+1}=d(i).product; %#ok<AGROW>
        allInfos{end+1}=d(i); %#ok<AGROW>
    end
end
