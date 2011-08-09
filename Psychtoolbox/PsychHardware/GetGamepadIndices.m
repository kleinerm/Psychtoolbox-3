function [gamepadIndices, productNames]= GetGamepadIndices;
% keyboardIndices = GetGamepadIndices
%
% OS X: ___________________________________________________________________
%
% PsychHID assigns each USB HID device connected to you computer a
% unique index. GetGamepadIndices returns the indices for those HID devices
% which are gamepads.  The product names of each gamepad are returned in a
% second argument which is useful to identify the gamepad associated with
% an index.  For complete information on a gampad use PsychHID('Devices').
%
% OS 9: ___________________________________________________________________
%
% GetGamepadIndices does not exist in OS 9. 
%
% WINDOWS: ________________________________________________________________
% 
% GetGamepadIndices does not exist in Windows.
% 
% _________________________________________________________________________
%
% see also: GetKeyboardIndices

% HISTORY
% 7/6/03    awi     Wrote it.
% 7/13/04   awi     Improved documentation
% 1/20/05   mk      Added fix for Logitech joystick (Forum msg. 4149)

gamepadIndices=[];
productNames=cell(0);

if ~IsOSX
  % On Linux, a mouse is a gamepad, is a mouse...
  d = [ PsychHID('Devices', 3) , PsychHID('Devices', 5) ];
else
  d=PsychHID('Devices');
end

for i =1:length(d);
    if IsLinux || (d(i).usagePageValue==1 && (d(i).usageValue == 5 || d(i).usageValue == 4))
        gamepadIndices(end+1)=d(i).index;
        productNames{end+1}=d(i).product;
    end
end
