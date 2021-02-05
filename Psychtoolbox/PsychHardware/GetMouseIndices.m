function [mouseIndices, productNames, allInfo] = GetMouseIndices(typeOnly, product, serialNumber, locationID)
% [mouseIndices, productNames, allInfo] = GetMouseIndices([typeOnly][, productName][, serialNumber][, locationID])
%
% OS X: ___________________________________________________________________
%
% PsychHID assigns each USB HID device connected to you computer a unique
% index. GetMouseIndices returns the indices for those HID devices which
% are mouses. The product names of the mouses are returned in a second
% argument which is useful to identify the mouse associated with a
% paticular index. For complete information on a gamepad use
% PsychHID('Devices').
%
% LINUX: __________________________________________________________________
%
% GetMouseIndices allows selection of different types of pointing devices
% via the optional 'typeOnly' argument:
% 'masterPointer' will only return indices of so called "master pointer"
% devices. These correspond to visible mouse cursors. 'slavePointer' will
% only return indices of slave pointer devices. If you want to use keyboard
% query functions like KbCheck, KbWait etc. to get mouse button presses,
% then you can only use slave pointer devices, ie., select between mice that
% are returned by setting 'typeOnly' as 'slavePointer'.
%
% WINDOWS: ________________________________________________________________
% 
% GetMouseIndices can not enumerate individual mice. All connected pointing
% devices are treated as one unified mouse.
% _________________________________________________________________________
%
% If you have multiple mice connected you can restrict the set of
% returned mouse by specifying the following optional match-critera:
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
% see also: GetKeyboardIndices, GetKeypadIndices, GetGamepadIndices

% HISTORY
% 10/15/04  awi   Wrote it. Based on GetGamepadIndices
% 12/17/09  rpw   Added see also for GetKeypadIndices
% 08/27/15  mk    Add 'slavePointer' support.
% 15-Aug-2017 mk  Add filtering by productName, serialNumber, locationID.
% 29-Sep-2017 mk  No masterPointer constraint no longer applies to KbQueues.
% 05-Feb-2021 mk  Only masterPointer on Windows atm.

mouseIndices=[];
productNames=cell(0);
allInfo=cell(0);

if nargin < 1
    typeOnly = [];
end

if nargin < 2
    product = [];
end

if nargin < 3
    serialNumber = [];
end

if nargin < 4
    locationID = [];
end

if ~IsOSX
    LoadPsychHID;
    if strcmpi(typeOnly, 'masterPointer') || IsWin
        d = PsychHID('Devices', 1);
    elseif strcmpi(typeOnly, 'slavePointer')
        d = PsychHID('Devices', 3);
    else
        d = [ PsychHID('Devices', 1) , PsychHID('Devices', 3) ];
    end
else
    d = PsychHID('Devices');
end

for i =1:length(d);
    if d(i).usagePageValue==1 && d(i).usageValue == 2
        % Check if additional match-criteria provided. Skip this device on mismatch:
        if ~isempty(product) && ~strcmpi(d(i).product, product)
            continue;
        end

        if ~isempty(serialNumber) && ~strcmpi(d(i).serialNumber, serialNumber)
            continue;
        end

        if ~isempty(locationID) && (d(i).locationID ~= locationID)
            continue;
        end

        mouseIndices(end+1)=d(i).index; %#ok<AGROW>
        productNames{end+1}=d(i).product; %#ok<AGROW>
        allInfo{end+1}=d(i); %#ok<AGROW>
    end
end
