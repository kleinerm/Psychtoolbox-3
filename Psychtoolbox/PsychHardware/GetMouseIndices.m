function [mouseIndices, productNames, allInfo]= GetMouseIndices(typeOnly)
% [mouseIndices, productNames, allInfo] = GetMouseIndices([typeOnly])
%
% OS X: ___________________________________________________________________
%
% PsychHID assigns each USB HID device connected to you computer a unique
% index. GetMouseIndices returns the indices for those HID devices which
% are mouses. The product names of the mouses are returned in a second
% argument which is useful to identify the mouse associated with a
% paticular index. For complete information on a gampad use
% PsychHID('Devices').
%
% LINUX: __________________________________________________________________
%
% GetMouseIndices allows selection of different types of pointing devices
% via the optional 'typeOnly' argument:
% 'masterPointer' will only return indices of so called "master pointer"
% devices. These correspond to visible mouse cursors. 'slavePointer' will
% only return indices of slave pointer devices. If you want to use keyboard
% query functions like KbCheck, KbQueueCheck etc. to get mouse button presses,
% then you can only use slave pointer devices, ie., select between mice that
% are returned by setting 'typeOnly' as 'slavePointer'.
%
% WINDOWS: ________________________________________________________________
% 
% GetMouseIndices works as on OS X.
% _________________________________________________________________________
%
% see also: GetKeyboardIndices, GetKeypadIndices, GetGamepadIndices

% HISTORY
% 10/15/04  awi   Wrote it. Based on GetGamepadIndices
% 12/17/09  rpw   Added see also for GetKeypadIndices
% 08/27/15  mk    Add 'slavePointer' support.

mouseIndices=[];
productNames=cell(0);
allInfo=cell(0);

if nargin < 1
  typeOnly = [];
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
    if d(i).usagePageValue==1 && d(i).usageValue == 2
        mouseIndices(end+1)=d(i).index; %#ok<AGROW>
        productNames{end+1}=d(i).product; %#ok<AGROW>
        allInfo{end+1}=d(i); %#ok<AGROW>
    end
end
