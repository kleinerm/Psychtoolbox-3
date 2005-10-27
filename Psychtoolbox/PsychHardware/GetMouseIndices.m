function [mouseIndices, productNames]= GetMouseIndices;
% mouseIndices = GetMouseIndices
%
% OS X: ___________________________________________________________________
%
% PsychHID assigns each USB HID device connected to you computer a unique
% index. GetMouseIndices returns the indices for those HID devices which
% are mouses.  The product names of the mouses are returned in a second
% argument which is useful to identify the mouse associated with a
% paticular index.  For complete information on a gampad use
% PsychHID('Devices').
%
% OS 9: ___________________________________________________________________
%
% GetMouseIndices does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% GetMouseIndices does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: GetKeyboardIndices, GetGamepadIndices

% HISTORY
% 10/15/04  awi     Wrote it.  Based on GetGamepadIndices


mouseIndices=[];
productNames=cell(0);
d=PsychHID('Devices');
for i =1:length(d);
    if d(i).usagePageValue==1 && d(i).usageValue == 2
        mouseIndices(end+1)=i;
        productNames{end+1}=d(i).product;
    end
end

