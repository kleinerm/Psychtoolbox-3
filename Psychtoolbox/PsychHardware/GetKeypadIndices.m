function [keypadIndices, productNames]= GetKeypadIndices
% keypadIndices= GetKeypadIndices
%
% OS X: ___________________________________________________________________
%
% The PsychHID assigns each USB HID device connected to you computer a
% unique index. GetKeypadIndices returns the indices for those HID
% devices which are keypads.  The product names of each keypad are
% returned in a second argument which is useful to identify the keypad
% associated with an index.  For complete information on a keypad use
% PsychHID('Devices').
%
% OS 9: ___________________________________________________________________
%
% GetKeypadIndices does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% GetKeypadIndices does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: GetGamepadIndices, GetKeyBoardIndices


% HISTORY
% 12/17/09    rpw     Wrote it. 

keypadIndices=[];
productNames=cell(0);
% Enumerate all HID devices:
if ~IsOSX
  % On Linux we only enumerate type 4 - slave keyboard devices. These are what we want:
  LoadPsychHID;
  d = PsychHID('Devices', 4);
else
  % On other OS'es enumerate everything and filter later:
  d = PsychHID('Devices');
end

for i =1:length(d);
    if d(i).usagePageValue==1 && d(i).usageValue == 7
        keypadIndices(end+1)=d(i).index; %#ok<AGROW>
        productNames{end+1}=d(i).product; %#ok<AGROW>
    end
end
