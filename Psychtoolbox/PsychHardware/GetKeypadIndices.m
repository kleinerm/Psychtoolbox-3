function [keypadIndices, productNames, allInfos] = GetKeypadIndices(product, serialNumber, locationID)
% [keypadIndices, productNames, allInfos] = GetKeypadIndices([productName][, serialNumber][, locationID])
%
% The PsychHID assigns each USB HID device connected to you computer a
% unique index. GetKeypadIndices returns the indices for those HID
% devices which are keypads. The product names of each keypad are
% returned in a second argument which is useful to identify the keypad
% associated with an index. The third return argument is a cell-array with
% complete information about the keypad device, e.g., allInfos{1} returns
% all known info about the 1st detected keypad.
%
% If you have multiple keypads connected you can restrict the set of
% returned keypads by specifying the following optional match-critera:
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
% _________________________________________________________________________
% WINDOWS: All keypads are treated as one keypad, you can not select between
% different keypads.
%
% see also: GetGamepadIndices, GetKeyBoardIndices

% HISTORY
% 17-Dec-2009 rpw Wrote it.
% 15-Aug-2017 mk  Add filtering by productName, serialNumber, locationID.
% 10-Apr-2018 mk  Filter out devices with locationID zero on broken macOS.

if nargin < 1
    product = [];
end

if nargin < 2
    serialNumber = [];
end

if nargin < 3
    locationID = [];
end

keypadIndices=[];
productNames=cell(0);
allInfos=cell(0);

% Enumerate all HID devices:
if ~IsOSX
    % On Linux we only enumerate type 4 - slave keypad devices. These are what we want:
    LoadPsychHID;
    d = PsychHID('Devices', 4);
else
    % On other OS'es enumerate everything and filter later:
    d = PsychHID('Devices');
end

for i =1:length(d);
    if d(i).usagePageValue==1 && d(i).usageValue == 7
        % Check if additional match-criteria provided. Skip this keypad device on mismatch:
        if ~isempty(product) && ~strcmpi(d(i).product, product)
            continue;
        end

        if ~isempty(serialNumber) && ~strcmpi(d(i).serialNumber, serialNumber)
            continue;
        end

        if ~isempty(locationID) && (d(i).locationID ~= locationID)
            continue;
        end

        % Protect against macOS brain-damage: Filter out the touchbar gadget.
        if IsOSX && d(i).locationID == 0
            continue;
        end

        % Match! Append to list of output keypads, including optional
        % detailed information:
        keypadIndices(end+1)=d(i).index; %#ok<AGROW>
        productNames{end+1}=d(i).product; %#ok<AGROW>
        allInfos{end+1}=d(i); %#ok<AGROW>
    end
end
