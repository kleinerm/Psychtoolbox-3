function [keyboardIndices, productNames]= GetKeyboardIndices;
% keyboardIndices= GetKeyboardIndices
%
% OS X: ___________________________________________________________________
%
% The PsychHID assigns each USB HID device connected to you computer a
% unique index. GetKeyboardIndices returns the indices for those HID
% devices which are keyboards.  The product names of each keyboard are
% returned in a second argument which is useful to identify the keyboard
% associated with an index.  For complete information on a keyboard use
% PsychHID('Devices').
%
% OS 9: ___________________________________________________________________
%
% GetKeyboardIndices does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% GetKeyboardIndices does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: GetGamepadIndices


% HISTORY
% 7/6/03    awi     Wrote it. 

keyboardIndices=[];
productNames=cell(0);
d=PsychHID('Devices');
for i =1:length(d);
    if d(i).usagePageValue==1 && d(i).usageValue == 6
        keyboardIndices(end+1)=i;
        productNames{end+1}=d(i).product;
    end
end

