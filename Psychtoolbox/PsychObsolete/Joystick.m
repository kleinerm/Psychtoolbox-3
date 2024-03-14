function result = Joystick(arg1, arg2, arg3)
% Joystick() is deprecated and will call GamePad() as its replacement.
% See 'help GamePad'.

if nargin>3
    error('Too many arguments');
elseif nargin==3
    tempResult=Gamepad(arg1,arg2,arg3);
elseif nargin==2
    tempResult=Gamepad(arg1,arg2);
elseif nargin==1
    tempResult=Gamepad(arg1);
elseif nargin==0
    Gamepad;
    tempResult=nan;
end

if ~isnan(tempResult)
    result=tempResult; 
end

warning('Psychtoolbox:Joystick', '"Joystick" has been renamed to "Gamepad". For improved performance use the new name, "Gamepad".');
warning('off', 'Psychtoolbox:Joystick');
