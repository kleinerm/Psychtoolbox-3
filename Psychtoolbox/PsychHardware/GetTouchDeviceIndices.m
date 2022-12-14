function [touchIndices, productNames, allInfo] = GetTouchDeviceIndices(typeOnly, touchTypeOnly, product, serialNumber, locationID)
% [touchIndices, productNames, allInfo] = GetTouchDeviceIndices([typeOnly][, touchTypeOnly][, productName][, serialNumber][, locationID])
%
% Return 'touchIndices' a set of handles to select touch input devices
% like touchscreens, tablets and touchpads for touch input with the
% TouchXXX functions (TouchQueueCreate, TouchEventGet etc.).
%
% Also returns corresponding productNames for the devices and detailed
% info in the allInfo struct-array.
%
% LINUX: __________________________________________________________________
%
% GetTouchDeviceIndices allows selection of different types of touch devices
% via the optional 'typeOnly' argument:
% 'masterPointer' will only return indices of so called "master pointer"
% devices. These correspond to visible mouse cursors. 'slavePointer' will
% only return indices of slave pointer devices. Often only 'slavePointer'
% devices work properly or with full functionality for touch devices, that's
% why typeOnly defaults to 'slavePointer' if the argument is omitted.
%
% Windows: ________________________________________________________________
%
% Basic device enumeration should work, but only true touchscreens are supported,
% not touchpads.
%
% OS X: ___________________________________________________________________
%
% This function currently returns nothing, as OSX does not support touch
% screens in a meaningful way as far as we know. And Psychtoolbox for OSX
% currently does not implement any special support for touchpads or such.
%
% _________________________________________________________________________
%
% If you have touch devices connected you can restrict the set of
% returned devices by specifying the following optional match-critera:
%
% typeOnly      = 'masterPointer' or 'slavePointer' or 'allPointers'. If left
%                 out, this will default to 'slavePointer', unless on MS-Windows,
%                 where it defaults to 'masterPointer'.
%
% touchTypeOnly = 0 for touchpads, 1 for true touchscreens.
%
% product       = Product name of target devices, as returned in 'productNames'.
%
% serialNumber  = Serial number of target devices. This is a text string,
%                 not a number!
%
% locationID    = Numeric id of where the device is connected to the
%                 computer. The number is supposed to be unique for a given
%                 connection port. E.g., the same number will be returned
%                 whenever the device is connected to the same USB port of
%                 the computer. The value should be persistent across
%                 reboots of the machine, but may not be persistent across
%                 operating system upgrades - or may not be persistent at
%                 all in case of os bugs. Your mileage may vary...
% __________________________________________________________________________________
% see also: GetKeyboardIndices, GetKeypadIndices, GetGamepadIndices, GetMouseIndices

% HISTORY
% 01-Oct-2017 mk  Wrote it. Based on GetMouseIndices.

touchIndices=[];
productNames=cell(0);
allInfo=cell(0);

if nargin < 1 || isempty(typeOnly)
    if IsWin
        typeOnly = 'masterPointer';
    else
        typeOnly = 'slavePointer';
    end
end

if nargin < 2 || isempty(touchTypeOnly)
    touchTypeOnly = [0, 1];
else
    if ~ismember(touchTypeOnly, [0, 1])
        error('Invalid touchTypeOnly parameter specified. Valid are 0 and 1.');
    end
end

if nargin < 3
    product = [];
end

if nargin < 4
    serialNumber = [];
end

if nargin < 5
    locationID = [];
end

if ~IsOSX
    LoadPsychHID;
    if strcmpi(typeOnly, 'masterPointer')
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
    if ((d(i).usagePageValue==1 && d(i).usageValue == 2) || (d(i).usagePageValue==13 && d(i).usageValue == 4)) && ...
        ismember(d(i).touchDeviceType, touchTypeOnly)
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

        touchIndices(end+1)=d(i).index; %#ok<AGROW>
        productNames{end+1}=d(i).product; %#ok<AGROW>
        allInfo{end+1}=d(i); %#ok<AGROW>
    end
end
