function [keyboardIndices, productNames, allInfos]= GetKeyboardIndices(product, serialNumber, locationID)
% [keyboardIndices, productNames, allInfos] = GetKeyboardIndices([productName][, serialNumber][, locationID])
%
% The PsychHID assigns each USB HID device connected to you computer a
% unique index. GetKeyboardIndices returns the indices for those HID
% devices which are keyboards.  The product names of each keyboard are
% returned in a second argument which is useful to identify the keyboard
% associated with an index. The third return argument is a cell-array with
% complete information about the keyboard device, e.g., allInfos{1} returns
% all known info about the 1st detected keyboard.
%
% If you have multiple keyboards connected you can restrict the set of
% returned keyboard by specifying the following optional match-critera:
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
%
% see also: GetGamepadIndices


% HISTORY
% 7/6/03    awi     Wrote it.
% 12/8/09    mk     Add matching logic for selection of subsets of devices.

if nargin < 1
    product = [];
end

if nargin < 2
    serialNumber = [];
end

if nargin < 3
    locationID = [];
end

% Init:
keyboardIndices=[];
productNames=cell(0);
allInfos=cell(0);

% Enumerate all HID devices:
if ~IsOSX
  % On Linux or Windows we only enumerate type 4 - slave keyboard devices. These are what we want:
  LoadPsychHID;
  d = PsychHID('Devices', 4);
else
  % On other OS'es enumerate everything and filter later:
  d = PsychHID('Devices');
end

% Iterate through all of them:
for i =1:length(d);
    % Keyboard or keyboard-like device?
    if d(i).usagePageValue==1 && d(i).usageValue == 6
        % Check if additional match-criteria provided. Skip this keyboard device on mismatch:
        if ~isempty(product) && ~strcmpi(d(i).product, product)
            continue;
        end
        
        if ~isempty(serialNumber) && ~strcmpi(d(i).serialNumber, serialNumber)
            continue;
        end
        
        if ~isempty(locationID) && (d(i).locationID ~= locationID)
            continue;
        end

        % Match! Append to list of output keybords, including optional
        % detailed information:
        keyboardIndices(end+1)=d(i).index; %#ok<AGROW>
        productNames{end+1}=d(i).product; %#ok<AGROW>
        allInfos{end+1}=d(i); %#ok<AGROW>
    end
end

return;
