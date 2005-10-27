function result = Gamepad(arg1, arg2, arg3)
% result = Gamepad(command [,arg1] [,arg2])
%
% Gamepad reads the state of any USB game controller. Gamepad was
% previously named "JoyStick", the name has been changed to agree with the
% USB Human Interface Device (HID) terminology.
%
% Gamepad provides built-in help.  For usage, enter "Gamepad" alone on the
% MATLAB command line. 
% 
% OSX: ___________________________________________________________________
%
% PsychGamePad uses the PsychHID function, which provides a universal
% interface to USB HID devices, including keyboards.
%
% On OS X Gamepad is an .m file but provides built-in help to match the
% behavior of Gamepad on other platforms where it is a mex file.  
%
% OS9: ___________________________________________________________________
%
% Gamepad is still named Joystick.
% 
% Joystick uses the Simple DirectMedia Layer (SDL) library which  is
% included under the terms of the LGPL license. Enter "LGPL" at the Matlab
% command  prompt to view the LGPL.  To learn more about SDL see
% www.libsdl.org.  
% 
% Source code for the Psychtoolbox Joystick extensions is available from
% www.psychtoolbox.org.   Upon request we will provide SDL source code,
% which is also available from www.libsdl.org.
% 
% Joystick requires:
% 
% -Apple GameSprockets extensions.  The GameSprockts installer is available
% from Apple at:
% http://kbase.info.apple.com/cgi-bin/WebObjects/kbase.woa/28/wa/query?sear
% chMode=Expert&type=id&val=KC.31236
% 
% -The SDL extension must be moved to your System:Extensions folder and
% your computer restarted.   If you used the Psychtoolbox installer this
% should have been done for you.  The SDL extension is included in the
% Psychtoolbox in the folder:
%    Matlab:Toolbox:Psychtoolbox:PsychHardware:Joystick.  
%   
% WIN: ________________________________________________________________
%
% Gamepad has been omitted from the WIN Psychtoolbox because of a bug that 
% prevents the extensions from loading.   
% _________________________________________________________________________
%
% See also: PsychGamepad, PsychHID


% HISTORY
% 7/21/03   awi     Wrote it.
% 7/13/04   awi     Fixed documentation.

% NOTES
%  7/21/03  awi     Gamepad does not support ball elements because I do not know what those are 
%                   and do not have a joystick with ball elements. If someone does, report the 
%                   the model of the joystick and I will add the feature.
%                   Note that the SDL source _should_
%                   internally idenfify the HID usage and usage page numbers for what it calls a 
%                   "joystick ball" but it does not, SDL on OSX seems not to support ball
%                   elements.  The SDL source always sets the number of ball elements to 0. 

% TO DO
%  7/21/03  awi     Check SDL source for other platforms, maybe some version contains the the HID usage
%                   for "joystick ball".  
%  7/22/03  awi     gamepadIndices and deviceStructs variables could be retained between invocations and 
%                   cleared with the 'unplug' command.
%	10/6/05  awi     Note here cosmetic changes by dgp between 7/22/03 and 10/6/05.

if IsOSX
    
	%device page and usage
	gamepadUsagePageValue=1;    gamepadUsageValue=5;
	
	%element page and usage
	axisUsagePageValue=1;       axisUsageValues=[48 49 50 51 52 53];
	sliderUsagePageValue=1;     sliderUsageValues=54;
	dialUsagePageValue=1;       dialUsageValues=55;
	wheelUsagePageValue=1;      wheelUsageValues=56;
	hatswitchUsagePageValue=1;  hastSwitchUsageValues=57;
	buttonUsagePageValue=9;     %anything on the button usage page is a button
	
	
	oldGamepadSubCommands= {'GetNumJoysticks', 'GetJoystickName', 'GetJoystickNumbersFromName', 'GetNumButtons', ...
        'GetNumAxes', 'GetNumBalls', 'GetNumHats', 'GetButton', 'GetAxis', 'GetBall', 'GetHat', 'Unplug'};
	newGamepadSubCommands= {'GetNumGamepads', 'GetGamepadNamesFromIndices', 'GetGamepadIndicesFromNames', 'GetNumButtons', ...
        'GetNumAxes', 'GetNumBalls', 'GetNumHats', 'GetButton', 'GetAxis', 'GetBall', 'GetHat', 'Unplug'};
	allGamepadSubCommands=union(oldGamepadSubCommands, newGamepadSubCommands);
    for i = 1:length(allGamepadSubCommands)
        allGamepadHelpRequests{i}=[allGamepadSubCommands{i} '?'];
    end
    allPossibleCommands=upper(union(allGamepadHelpRequests, allGamepadSubCommands));
	
	if nargin == 0
        fprintf('Usage:\n');
        fprintf('\n');
        fprintf('%%Get information on gamepad availability.\n');
        fprintf('numGamepads = Gamepad(''GetNumGamepads'')\n');
        fprintf('gamepadName = Gamepad(''GetGamepadNamesFromIndices'', gamepadIndices)\n');
        fprintf('gamepadIndices = Gamepad(''GetGamepadIndicesFromNames'', joystickName)\n');
        fprintf('\n');
        fprintf('%%Get information on gamepad configuration.\n');
        fprintf('numButtons = Gamepad(''GetNumButtons'', gamepadIndex)\n');
        fprintf('numAxes = Gamepad(''GetNumAxes'', gamepadIndex)\n');
        fprintf('numBalls = Gamepad(''GetNumBalls'', gamepadIndex)\n');
        fprintf('numHats = Gamepad(''GetNumHats'', gamepadIndex)\n');
        fprintf('\n');
        fprintf('%%Read gamepad states.\n');
        fprintf('buttonState = Gamepad(''GetButton'', gamepadIndex, buttonIndex)\n');
        fprintf('axisState = Gamepad(''GetAxis'', gamepadIndex, axisIndex)\n');
        fprintf('[deltaX, deltaY]  = Gamepad(''GetBall'', gamepadIndex, ballIndex)\n');
        fprintf('hatPosition = Gamepad(''GetHat'', gamepadIndex, hatIndex)\n');
        fprintf('\n');
        fprintf('%%Unplug a gamepad.\n');
        fprintf('Gamepad(''Unplug'')\n');
        fprintf('\n');
        return;
	end
	
	
	%for backwards compatability with Joystick we reshuffle arguments. Note
	%that the shortcircuited boolean operators are there to protect tests from 
	%invalid non-char argument types.
	if nargin==1
        if ~ismember(upper(arg1), upper(allPossibleCommands))
            error('unrecognized subcommand');
        else
            subCommand=upper(arg1);
            argVal1=[];
            argVal2=[];
            numArgVals=0;
        end
	elseif nargin > 3
        error('too many arguments')
	else  %either two or three arguments
        %command is given as both first and second arguments
        if ischar(arg1) && ischar(arg2) && ismember(upper(arg1), upper(allPossibleCommands)) && ismember(upper(arg2), upper(allPossibleCommands))
            error('subcommand repeated in both first and second arguments');
        elseif (~ischar(arg1) || ~ismember(upper(arg1), upper(allPossibleCommands))) && (~ischar(arg2) || ~ismember(upper(arg2), upper(allPossibleCommands)))
            error('no recognized subcommnad');
        elseif ischar(arg1) && ismember(upper(arg1), upper(allPossibleCommands))
            subCommand=upper(arg1);
            argVal1=arg2;
            numArgVals=1;
            argVal2=[];
            if nargin==3
                numArgVals=2;
                argVal2=arg3;
            end
        elseif ischar(arg2) && ismember(upper(arg2), upper(allPossibleCommands))
            subCommand=upper(arg2);
            argVal1=arg1;
            numArgVals=1;
            argVal2=[];
            if nargin==3
                numArgVals=2;
                argVal2=arg3;
            end
        end
	end
    
    %display command-specific help strings and return.  This is not the
    %usual way to provide documenation in .m function, but Gamepad
    %emulates the old Joystick mex, so we we do this.
    if ismember(upper(subCommand), upper(allGamepadHelpRequests))
        switch upper(subCommand)
            case{'GETNUMJOYSTICKS?', 'GETNUMGAMEPADS?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('numGamepads = Gamepad(''GetNumGamepads'')\n');
                fprintf('\n');
                fprintf(' Get the number of gamepads connected to the system. Each gamepad is assigned\n');
                fprintf(' an index in the range of 1 to the value returned by ''GetNumGamepads''. Use the\n');
                fprintf(' assigned index to refer to a specific gamepad when calling Gamepad subfunctions\n');
                fprintf(' which read the gamepad state, such as ''GetButtons''.  Assigned indices change when\n');
                fprintf(' you unplug or add gamepads, or reboot your computer. Therefore, you should not\n');
                fprintf(' code index values into your scripts.  Instead, use Gamepad subfunction\n');
                fprintf(' ''GetGamepadIndexFromName'' to identify a particular model of gamepad.\n');
                fprintf('\n');
            case{'GETJOYSTICKNAME?', 'GETGAMEPADNAMESFROMINDICES?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('GamepadNames = Gamepad(''GetGamepadNamesFromNumbers'', gamepadIndices\n');
                fprintf('\n');
                fprintf('Given a vector of gamepad indices return a cell array of gamepad names.  The gamepad\n');
                fprintf('name is the same as the product field of the HID device structure reurned by PsychHID,\n'); 
                fprintf('the name of the device assigned by the manufacturer; Identical joysticks should report\n'); 
                fprintf('the same device name. Gamepads are indexed beginning with 1 up to the number of\n'); 
                fprintf('connected joysticks. Use Gamepad(''GetNumGamepads'') to find the number of connected\n');
                fprintf('gamepads.\n');
                fprintf('\n');
            case{'GETJOYSTICKNUMBERSFROMNAME?', 'GETGAMEPADINDICESFROMNAMES?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('gamepadIndices = Gamepad(''GetGamepadNumbersFromNames'', gamepadNames)\n');
                fprintf('\n');
                fprintf('Given string or cell array of strings holding gamepad names return a vector\n'); 
                fprintf('of the indices of all gameads with those names.   The gamepad name is the same as the\n');
                fprintf('product field of the HID device structure reurned by PsychHID, the name of the device \n'); 
                fprintf('assigned by the manufacturer; Identical joysticks should report\n'); 
                fprintf('the same device name.\n');
                fprintf('\n');
            case{'GETNUMBUTTONS?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('numButtons = Gamepad(''GetNumButtons'', gamepadIndex)\n');
                fprintf('\n');
                fprintf('Given a gamepad number return the number of buttons on the gamepad.\n');
                fprintf('Gamepad(''GetButton'' will return the state of any button.\n');
                fprintf('\n');
            case{'GETNUMAXES?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('\n');
                fprintf('numAxes = Gamepad(''GetNumAxes'', gamepadIndex)\n');
                fprintf('Given a gamepad index return the number of Axes on the gamepad.\n');
                fprintf('Gamepad(''GetAxis'') will return the state of any axis.\n');
                fprintf('\n');
            case{'GETNUMBALLS?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('\n');
                fprintf('numBalls = Gamepad(''GetNumBalls'', gamepadIndex)\n');
                fprintf('Given a gamepad index return the number of balls on the gamepad.\n');
                fprintf('Gamepad(''GetBall'') will return the state of any ball.\n');
                fprintf('\n');
            case{'GETNUMHATS?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('numHats = Gamepad(''GetNumHats'', gamepadIndex)\n');
                fprintf('\n');
                fprintf('Given a gamepad index return the number of hats on the gamepad.\n');
                fprintf('Gamepad(''GetHat'') will return the state of any hat.\n');
                fprintf('\n');
            case{'GETBUTTON?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('buttonState = Gamepad(''GetButton'',gamepadIndex, buttonIndex\n');
                fprintf('\n');
                fprintf('Given a gamepad index and a button index, return the immediate state of the\n');
                fprintf('specified button on the specified gamepad.  Use ''GetNumGamepads'' and\n');
                fprintf('''GetNumButtons'' to find the maximum legal values for gamepadIndex and\n');
                fprintf('buttonIndex arguments. The first gamepad and the first button are index 1. It\n');
                fprintf('is an error to call ''GetButton'' if the specified gamepad has no buttons.\n');
                fprintf('\n');
            case{'GETAXIS?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('axisState = Gamepad(''GetAxis'', gamepadIndex,axisIndex)\n');
                fprintf('\n');
                fprintf('Given a gamepad index and an axis index, return the immediate state of the\n');
                fprintf('specified axis on the specified gamepad.  The returned value axisState is an\n');
                fprintf('integer (-32768 to 32768) representing the current position of the axis. It\n');
                fprintf('might be necessary to impose tolerances on the axisState value to ignore\n');
                fprintf('jitter.Use ''GetNumGamepads'' and ''GetNumAxes'' to determine maximum legal values\n');
                fprintf('for gamepadIndex and axisIndex arguments. The first gamepad\n');
                fprintf('and the first axis are numbered 1. It is an error to call ''GetAxis'' if the\n');
                fprintf('specified gamepad has no axes.\n');
                fprintf('\n');
            case{'GETBALL?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('[deltaX, deltaY]  = Gamepad(''GetBall'',gamepadIndex,ballIndex)\n');
                fprintf('\n');
                fprintf('Given a gamepad index and a trackball index, return the change in the\n');
                fprintf('trackball X and Y positions since the previous call to ''GetBall''.\n');
                fprintf('Use ''GetNumGamepads'' and ''GetNumBalls'' to determine maximum legal values for\n');
                fprintf('gamepadIndex and gallIndex arguments. The first gamepad and\n');
                fprintf('the first ball are numbered 1. It is an error to call ''GetBall'' if the specified\n');
                fprintf('gamepad has no ball elements.  On OSX Gamepad does not support ball elements. If\n');
                fprintf('you have a gamepad with a ball, report the gamepad model and we will add support.\n');
                fprintf('\n');
            case{'GETHAT?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('hatState = Gamepad(''GetHat'',gamepadIndex, hatIndex\n');
                fprintf('\n');
                fprintf('Given a gamepad index and a hat index, return the immediate state of the\n');
                fprintf('specified hat on the specified gamepad.  Use ''GetNumGamepads'' and\n');
                fprintf('''GetNumHats'' to find the maximum legal values for gamepadIndex and\n');
                fprintf('hatIndex arguments. The first gamepad and the first hat are index 1. It\n');
                fprintf('is an error to call ''GetHat'' if the specified gamepad has no hats.\n');
                fprintf('hatState is an ineger in the range 1-9 indicting at which of eight \n');
                fprintf('compass points or center the hat switch is positioned.  see "help Gamepad"\n');
                fprintf('for a list of functions such as "HatUp" which return constants for named\n');
                fprintf('compass points.\n');
                fprintf('\n');
            case{'UNPLUG?'}
                fprintf('Usage:\n');
                fprintf('\n');
                fprintf('Gamepad(''Unplug'')\n');
                fprintf('\n');
                fprintf('Resets the internal state of Gamepad function.  Call before plugging or\n');
                fprintf('unplugging gamepads\n');
                fprintf('\n');
        end %switch
        if nargout==1
            result=nan;  %if invoked by Joystick wrapper
        end
        return
    end
  
	
	gamepadIndices=GetGamepadIndices;
	deviceStructs=PsychHID('Devices');
	switch upper(subCommand)
        case {'GETNUMJOYSTICKS','GETNUMGAMEPADS'}
            if numArgVals > 0
                error('too many arguments supplied'); 
            end
            result = length(gamepadIndices);
            return
        case {'GETJOYSTICKNAME', 'GETGAMEPADNAMESFROMINDICES'}
            if numArgVals ~= 1
                error('incorrect number of arguments supplied');
            end
            for i = 1:length(argVal1)
                if argVal1(i) > length(gamepadIndices)
                    error('invalid gamepad index');
                end
                nameArray{i}=deviceStructs(gamepadIndices(argVal1(i))).product;            
            end
            result=nameArray;
            return
        case {'GETJOYSTICKNUMBERSFROMNAME', 'GETGAMEPADINDICESFROMNAMES'}
            if numArgVals ~= 1
                error('incorrect number of arguments supplied');
            end
            if ~(iscell(argVal1) || ischar(argVal1))
                error('wrong argument type supplied; type char or cell array expected');
            end
            numList=[];
            if ischar(argVal1)
                argVal1={argVal1};
            end
            %iterate over list of device  names
            for i = 1:length(argVal1)
                if ~ischar(argVal1{i})
                    error('supplied cell array argument has wrong element type, char expected');
                end
                %iterate over list of joystick indices
                for j =1:length(gamepadIndices)
                    if strcmpi(argVal1{i}, deviceStructs(gamepadIndices(j)).product)
                        numList=[numList j];
                    end
                end
            end
            result=numList;
            return
        case {'GETNUMBUTTONS', 'GETNUMAXES', 'GETNUMBALLS', 'GETNUMHATS'}
            if numArgVals ~= 1
                error('incorret number of arguments supplied');
            end
            if ~isa(argVal1, 'double') || length(argVal1) ~= 1
                error('1x1 double argument expected');
            end
            if argVal1 > length(gamepadIndices)
                error('invalid gamepad index');
            end
            switch upper(subCommand)
                case {'GETNUMBUTTONS'}
                    result = deviceStructs(gamepadIndices(argVal1)).buttons;
                case {'GETNUMAXES'}
                    result = deviceStructs(gamepadIndices(argVal1)).axes;
                case {'GETNUMBALLS'}
                    result = 0;    %Don't know what are the HID usage and usage page values for what SDL calls a "ball".  The OSX SDL source does not say.    
                case {'GETNUMHATS'}
                    result = deviceStructs(gamepadIndices(argVal1)).hats;
            end
            return
        case {'GETBUTTON'}
            if numArgVals ~= 2
                error('incorret number of arguments supplied');
            end
             if ~isa(argVal1, 'double') || length(argVal1) ~= 1 || ~isa(argVal2, 'double') || length(argVal2) ~= 1
                error('1x1 double argument expected');
            end
            if argVal1 > length(gamepadIndices)
                error('invalid gamepad index');
            end
            if argVal2 > deviceStructs(gamepadIndices(argVal1)).buttons
                error('invalid button index');
            end
            elements=PsychHID('Elements',gamepadIndices(argVal1));
            %get the indices of the button elements.  It would be faster to implement this within PsychHID.
            buttonIndices=[];
            for(i=1:length(elements))
                if elements(i).usagePageValue==buttonUsagePageValue
                    buttonIndices=[buttonIndices i];
                end
            end
            if deviceStructs(gamepadIndices(argVal1)).buttons ~= length(buttonIndices)
                error('found more elements of type button than the reported number of buttons');
            end
            result = PsychHID('RawState', gamepadIndices(argVal1), buttonIndices(argVal2));
            return
        case {'GETAXIS'}
            if numArgVals ~= 2
                error('incorret number of arguments supplied');
            end
             if ~isa(argVal1, 'double') || length(argVal1) ~= 1 || ~isa(argVal2, 'double') || length(argVal2) ~= 1
                error('1x1 double arguments expected');
            end
            if argVal1 > length(gamepadIndices)
                error('invalid gamepad index');
            end
            if argVal2 > deviceStructs(gamepadIndices(argVal1)).axes
                error('invalid axis index')
            end
            elements=PsychHID('Elements',gamepadIndices(argVal1));
            %get the indices of the axis elements.  It would be faster to implement this within PsychHID.
            axisIndices=[];
            for(i=1:length(elements))
                if elements(i).usagePageValue==axisUsagePageValue && any(ismember(elements(i).usageValue, axisUsageValues))
                    axisIndices=[axisIndices i];
                end
            end
            rawState = PsychHID('RawState', gamepadIndices(argVal1), axisIndices(argVal2));
            hidMin=elements(axisIndices(argVal2)).rangeMin;
            hidMax=elements(axisIndices(argVal2)).rangeMax;
            joyMin=AxisMin;
            joyMax=AxisMax;
            result = (rawState-hidMin)/(hidMax-hidMin) * (joyMax-joyMin) + joyMin; 
            return
        case {'GETHAT'}
            if numArgVals ~= 2
                error('incorret number of arguments supplied');
            end
             if ~isa(argVal1, 'double') || length(argVal1) ~= 1 || ~isa(argVal2, 'double') || length(argVal2) ~= 1
                error('1x1 double argument expected');
            end
            if argVal1 > length(gamepadIndices)
                error('invalid gamepad index');
            end
            if argVal2 > deviceStructs(gamepadIndices(argVal1)).hats
                error('invalid hat index')
            end
            elements=PsychHID('Elements',gamepadIndices(argVal1));
            %get the indices of the hat elements.  It would be faster to implement this within PsychHID.
            hatIndices=[];
            for(i=1:length(elements))
                if elements(i).usagePageValue==hatswitchUsagePageValue && any(ismember(elements(i).usageValue, hastSwitchUsageValues))
                    hatIndices=[hatIndices i];
                end
            end
            rawState = PsychHID('RawState', gamepadIndices(argVal1), hatIndices(argVal2));
            result=rawState;
            return
        case {'GETBALL'}
            if numArgVals ~= 2
                error('incorret number of arguments supplied');
            end
             if ~isa(argVal1, 'double') || length(argVal1) ~= 1 || ~isa(argVal2, 'double') || length(argVal2) ~= 1
                error('1x1 double argument expected');
            end
            if argVal1 > length(gamepadIndices)
                error('invalid gamepad index');
            end
            %we don't support balls because we do not know what they are. 
            error('invalid ball index');   
            return
        case {'UNPLUG'}
            clear PsychHID
            return
	end %switch

end %if IsOSX






