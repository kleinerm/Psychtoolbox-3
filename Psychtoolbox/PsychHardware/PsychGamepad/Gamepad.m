function result = Gamepad(arg1, arg2, arg3)
% result = Gamepad(command [,arg1] [,arg2])
%
% Gamepad reads the state of any USB game controller. Gamepad was
% previously named "JoyStick", the name has been changed to agree with the
% USB Human Interface Device (HID) terminology.
%
% Gamepad provides built-in help.  For usage, enter "Gamepad" alone on the
% command line.
%
% Performance tips:
%
% As initialization of Gamepad takes significant time, you should call it
% once at the beginning of your script before entering the trial loop.
% After plugging/unplugging or replugging of any USB devices you *must*
% call Gamepad('Unplug') or clear all, so Gamepad can recognize the changed
% hardware configuration at next invocation. Actually, a clear all is
% recommended, as Gamepad('Unplug') doesn't work reliably on non-OSX systems.
%
% The subfunctions for state queries of axis, button and hat state are
% optimized for low execution time -- suitable for time critical parts of
% your trial loop. All other functions, e.g., for query of the number of
% buttons etc. are not optimized, so they shouldn't be called in time
% critical parts of your loop.
%
% If you need ultra-fast queries, check the help for 'GetButtonRawMapping',
% 'GetAxisRawMapping' and 'GetHatRawMapping' for a slightly inconvenient,
% but superfast way to query device state.
%
%
% LINUX: ______________________________________________________________________
%
% Gamepad uses the Screen() mex file and its mouse query functions.
% On Linux, gamepads and joysticks are treated as a special type of
% mouse/pointing device with multiple extra axes and buttons. If your
% GamePad is not recognized, you may need to install the joystick driver,
% e.g., via "sudo apt-get install xserver-xorg-input-joystick" on a Debian/
% Ubuntu system, "sudo apt-get install xserver-xorg-input-joystick-lts-utopic"
% on some Ubuntu 14.04.2 LTS variants, or "sudo apt-get install xserver-xorg-input-joystick-lts-vivid"
% on some Ubuntu 14.04.3 LTS variants, or xf86-input-joystick on systems still
% using xf86. You may also wish to install a custom joystick configuration file
% to customize the mapping  and behaviour of buttons and axis, and if the
% Joystick also operates as a mouse or not.
%
% An example configuration file with installation instructions is 
% available in the Psychtoolbox/PsychContributed folder under the name 
% "52-MyLinuxJoystick.conf".
%
% OSX: ________________________________________________________________________
%
% GamePad uses the PsychHID function, which provides a universal
% interface to USB HID devices, including keyboards.
%
% On OS X Gamepad is an .m file but provides built-in help to match the
% behavior of Gamepad on other platforms where it is a mex file.
%
% WIN: ________________________________________________________________________
%
% Gamepad is not yet supported on Windows. WinJoystickMex() in the
% Psychtoolbox/PsychContributed subfolder may serve as a temporary
% replacement.
% _____________________________________________________________________________
%
% See also: PsychGamepad, PsychHID


% HISTORY
% 7/21/03   awi     Wrote it.
% 7/13/04   awi     Fixed documentation.
% 10/30/07  mk      Optimizations: Now 50 times faster due to caching and
%                   restructuring. Also new low-level access commands that
%                   allow another 7x speedup at expense of more complex
%                   usage.
% 08/05/11  mk      Allow use on Linux as well.

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
%  10/6/05  awi     Note here cosmetic changes by dgp between 7/22/03 and 10/6/05.

persistent axisUsagePageValue axisUsageValues hatswitchUsagePageValue hastSwitchUsageValues buttonUsagePageValue allGamepadSubCommands allGamepadHelpRequests joyMin joyMax gamepadIndices deviceStructs gi ei

if isempty(deviceStructs)
    % First call: Don't have cached data. Build all device info
    % structures by device detection and enumeration. Initialize all
    % other frequently used data structures...

    % Check if we're called on a supported OS. Currently Windows is
    % unsupported:
    if IsWin
        error('Sorry, the Gamepad function does not work on Windows yet.');
    end

    % Constants for device page and usage
    % gamepadUsagePageValue=1;    gamepadUsageValue=5;

    %element page and usage
    axisUsagePageValue=1;       axisUsageValues=[48 49 50 51 52 53];
    % sliderUsagePageValue=1;     sliderUsageValues=54;
    % dialUsagePageValue=1;       dialUsageValues=55;
    % wheelUsagePageValue=1;      wheelUsageValues=56;
    hatswitchUsagePageValue=1;  hastSwitchUsageValues=57;
    buttonUsagePageValue=9;     %anything on the button usage page is a button

    joyMin=AxisMin;
    joyMax=AxisMax;

    % Subcommand names etc.:
    oldGamepadSubCommands= {'GetNumJoysticks', 'GetJoystickName', 'GetJoystickNumbersFromName', 'GetNumButtons', ...
        'GetNumAxes', 'GetNumBalls', 'GetNumHats', 'GetButton', 'GetAxis', 'GetBall', 'GetHat', 'Unplug'};
    newGamepadSubCommands= {'GetNumGamepads', 'GetGamepadNamesFromIndices', 'GetGamepadIndicesFromNames', 'GetNumButtons', ...
        'GetNumAxes', 'GetNumBalls', 'GetNumHats', 'GetButton', 'GetAxis', 'GetBall', 'GetHat', 'Unplug', 'GetAxisRawMapping', 'GetButtonRawMapping', 'GetHatRawMapping'};
    allGamepadSubCommands=union(oldGamepadSubCommands, newGamepadSubCommands);
    for i = 1:length(allGamepadSubCommands)
        allGamepadHelpRequests{i}=[allGamepadSubCommands{i} '?'];
    end
    % allPossibleCommands=upper(union(allGamepadHelpRequests, allGamepadSubCommands));

    % Device detection and enumeration:
    [gamepadIndices, gamepadNames] = GetGamepadIndices;
    
    if IsLinux
      % Linux method: Gamepads are treated as slave mouse devices with extra
      % buttons and axis. Use GetMouse to get info about them:

      % Query additional per-device configuration info and parameters:
      for i=1:length(gamepadIndices)
        [x,y,b,f,v,vi] = GetMouse([], gamepadIndices(i));
        deviceStructs(gamepadIndices(i)).buttonIndices = 1:(length(b) - 32);
        deviceStructs(gamepadIndices(i)).axisIndices = 1:length(v);
        deviceStructs(gamepadIndices(i)).hatIndices = [];
        deviceStructs(gamepadIndices(i)).product = char(gamepadNames(i));
        deviceStructs(gamepadIndices(i)).buttons = length(b) - 32;
        deviceStructs(gamepadIndices(i)).axes = length(v);
        deviceStructs(gamepadIndices(i)).balls = 0;
        deviceStructs(gamepadIndices(i)).hats = 0;

        % Populate element properties:
        elements = [];
        for j = 1:length(vi)
          el.rangeMin = vi(j).min;
          el.rangeMax = vi(j).max;
          el.label = vi(j).label;
          if isempty(elements)
            elements = el;
          else
            elements(end+1) = el;
          end
        end
        deviceStructs(gamepadIndices(i)).elements = elements;
      end
    end

    if IsOSX
    % The following is OS/X only stuff:
    deviceStructs=PsychHID('Devices');

    % Query additional per-device configuration info and parameters:
    for i=1:length(gamepadIndices)
        elements = PsychHID('Elements', gamepadIndices(i));
        deviceStructs(gamepadIndices(i)).elements = elements;

        % Get the indices of the button elements.
        buttonIndices=[];
        for j=1:length(elements)
            if ~isempty(elements(j).usagePageValue) && (elements(j).usagePageValue==buttonUsagePageValue)
                buttonIndices=[buttonIndices j]; %#ok<AGROW>
            end
        end

        if deviceStructs(gamepadIndices(i)).buttons ~= length(buttonIndices)
            error('Found more elements of type button than the reported number of buttons');
        end

        deviceStructs(gamepadIndices(i)).buttonIndices = buttonIndices;

        % Get the indices of the axis elements.
        axisIndices=[];
        for j=1:length(elements)
            if ~isempty(elements(j).usagePageValue) && (elements(j).usagePageValue==axisUsagePageValue) && any(ismember(elements(j).usageValue, axisUsageValues))
                axisIndices=[axisIndices j]; %#ok<AGROW>
            end
        end

        deviceStructs(gamepadIndices(i)).axisIndices = axisIndices;

        % Get the indices of the hat elements.
        hatIndices=[];
        for j=1:length(elements)
            if ~isempty(elements(j).usagePageValue) && (elements(j).usagePageValue==hatswitchUsagePageValue) && any(ismember(elements(j).usageValue, hastSwitchUsageValues))
                hatIndices=[hatIndices j]; %#ok<AGROW>
            end
        end
        deviceStructs(gamepadIndices(i)).hatIndices = hatIndices;
    end
    end % OSX

    % End of first-time initialization.
end

% Command dispatch: For speed reasons, we first check for the most
% frequently used - and most time critical - configuration, the one
% that's most likely to be inside a realtime trial loop:

% Step 1: Argument assignment and remapping for compatibility with old
% Joystick function...

if nargin == 3
    % Commands with three arguments perform query of device state:
    if ischar(arg1)
        subCommand=upper(arg1);
        argVal1=arg2;
        argVal2=arg3;
    else
        subCommand=upper(arg2);
        argVal1=arg1;
        argVal2=arg3;
    end
else
    if nargin == 2
        % Command with two arguments: Configuration query:
        argVal2=[];
        if ischar(arg1)
            subCommand=upper(arg1);
            argVal1=arg2;
        else
            subCommand=upper(arg2);
            argVal1=arg1;
        end
    else
        if nargin == 1
            subCommand=upper(arg1);
            argVal1=[];
            argVal2=[];
        else
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
                fprintf('%%Retrieve low-level handles for fast raw access to elements.\n')
                fprintf('handles = Gamepad(''GetButtonRawMapping'', gamepadIndex, buttonIndex)\n');
                fprintf('handles = Gamepad(''GetAxisRawMapping'', gamepadIndex, axisIndex)\n');
                fprintf('handles = Gamepad(''GetHatRawMapping'', gamepadIndex, hatIndex)\n');
                fprintf('\n');        
                fprintf('%%Unplug a gamepad.\n');
                fprintf('Gamepad(''Unplug'')\n');
                fprintf('\n');
                return;
            else
                error('Invalid number of arguments (greater than three) provided!');
            end
        end
    end
end

% Step 2: Subcommand dispatch. Again, three parameter "realtime"
% commands are first - in case this matters for switch-times:
switch subCommand    
    case {'GETBUTTON'}
        % Argument checking is too expensive for realtime-path. We
        % 'try' if command works. On failure due to invalid params, the
        % 'catch' pathway will provide post-hoc explanation why it
        % failed.
        try
            gi = gamepadIndices(argVal1);
            ei = deviceStructs(gi).buttonIndices(argVal2);
            if IsLinux
                [d1,d2,b] = GetMouse([], gi);
                result = b(ei);
            else
                result = PsychHID('RawState', gi, ei);
            end
        catch
            if nargin ~= 3
                error('incorrect number of arguments supplied');
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
        end

        return

    case {'GETAXIS'}
        try
            gi = gamepadIndices(argVal1);
            ei = deviceStructs(gi).axisIndices(argVal2);
            if IsLinux
                [d1,d2,d3,d4,v] = GetMouse([], gi);
                rawState = v(ei);
            else
                rawState = PsychHID('RawState', gi, ei);
            end
            hidMin=deviceStructs(gi).elements(ei).rangeMin;
            hidMax=deviceStructs(gi).elements(ei).rangeMax;
            result = (rawState-hidMin)/(hidMax-hidMin) * (joyMax-joyMin) + joyMin;
        catch
            if nargin ~= 3
                error('incorrect number of arguments supplied');
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
        end
        return

    case {'GETHAT'}
        try
            gi = gamepadIndices(argVal1);
            ei = deviceStructs(gi).hatIndices(argVal2);
            if IsLinux
                error('Sorry, GetHat function unsupported on Linux.');
            else
                result = PsychHID('RawState', gi, ei);
            end
        catch
            if nargin ~= 3
                error('incorrect number of arguments supplied');
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
        end

        return

    case {'GETBALL'}
        if nargin ~= 3
            error('incorrect number of arguments supplied');
        end
        if ~isa(argVal1, 'double') || length(argVal1) ~= 1 || ~isa(argVal2, 'double') || length(argVal2) ~= 1
            error('1x1 double argument expected');
        end
        if argVal1 > length(gamepadIndices)
            error('invalid gamepad index');
        end
        %we don't support balls because we do not know what they are.
        error('Sorry, GetBall function not implemented yet.');

    case {'GETAXISRAWMAPPING'}
        % We call ourselves on the corresponding slow-path recursively.
        % This will trigger error-checking etc. and store the relevant raw
        % handles inside gi and ei.
        x = Gamepad('GetAxis', argVal1, argVal2);
        
        % Return the handles:
        result = [gi ei];
        return;
        
    case {'GETBUTTONRAWMAPPING'}
        % We call ourselves on the corresponding slow-path recursively.
        % This will trigger error-checking etc. and store the relevant raw
        % handles inside gi and ei.
        x = Gamepad('GetButton', argVal1, argVal2);
        
        % Return the handles:
        result = [gi ei];
        return;

    case {'GETHATRAWMAPPING'}
        % We call ourselves on the corresponding slow-path recursively.
        % This will trigger error-checking etc. and store the relevant raw
        % handles inside gi and ei.
        x = Gamepad('GetHat', argVal1, argVal2);
        
        % Return the handles:
        result = [gi ei];
        return;

    case {'GETNUMJOYSTICKS','GETNUMGAMEPADS'}
        if nargin > 1
            error('too many arguments supplied');
        end
        result = length(gamepadIndices);
        return

    case {'GETJOYSTICKNAME', 'GETGAMEPADNAMESFROMINDICES'}
        if nargin ~= 2
            error('incorrect number of arguments supplied');
        end
        for i = 1:length(argVal1)
            if argVal1(i) > length(gamepadIndices)
                error('invalid gamepad index');
            end
            nameArray{i}=deviceStructs(gamepadIndices(argVal1(i))).product;             %#ok<AGROW>
        end
        result=nameArray;
        return

    case {'GETJOYSTICKNUMBERSFROMNAME', 'GETGAMEPADINDICESFROMNAMES'}
        if nargin ~= 2
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
                    numList=[numList j]; %#ok<AGROW>
                end
            end
        end
        result=numList;
        return

    case {'GETNUMBUTTONS', 'GETNUMAXES', 'GETNUMBALLS', 'GETNUMHATS'}
        if nargin ~= 2
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
                result = length(deviceStructs(gamepadIndices(argVal1)).buttonIndices);
            case {'GETNUMAXES'}
                result = length(deviceStructs(gamepadIndices(argVal1)).axisIndices);
            case {'GETNUMBALLS'}
                result = 0;    %Don't know what are the HID usage and usage page values for what SDL calls a "ball".  The OSX SDL source does not say.
            case {'GETNUMHATS'}
                result = length(deviceStructs(gamepadIndices(argVal1)).hatIndices);
        end
        return

    case {'UNPLUG'}
        if IsOctave
           warning('Tried to call Gamepad(''Unplug''); which is unsupported under Octave. Ignored.');
           return;
        end

        if IsLinux
           warning('Called Gamepad(''Unplug''); - On Linx this implies a ''clear Screen'' aka closing all windows.');
           clear Screen;
        end

        clear PsychHID;
        clear GamePad;
        return

end %switch

% No "action" subcommand detected. Let's check if its a request for
% online help instead:

%display command-specific help strings and return.  This is not the
%usual way to provide documenation in .m function, but Gamepad
%emulates the old Joystick mex, so we we do this.
if ismember(upper(subCommand), upper(allGamepadHelpRequests))
    if nargout==1
        result=nan;  %if invoked by Joystick wrapper
    end

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
            return
        case{'GETJOYSTICKNAME?', 'GETGAMEPADNAMESFROMINDICES?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('GamepadNames = Gamepad(''GetGamepadNamesFromNumbers'', gamepadIndices\n');
            fprintf('\n');
            fprintf('Given a vector of gamepad indices return a cell array of gamepad names.  The gamepad\n');
            fprintf('name is the same as the product field of the HID device structure returned by PsychHID,\n');
            fprintf('the name of the device assigned by the manufacturer; Identical joysticks should report\n');
            fprintf('the same device name. Gamepads are indexed beginning with 1 up to the number of\n');
            fprintf('connected joysticks. Use Gamepad(''GetNumGamepads'') to find the number of connected\n');
            fprintf('gamepads.\n');
            fprintf('\n');
            return
        case{'GETJOYSTICKNUMBERSFROMNAME?', 'GETGAMEPADINDICESFROMNAMES?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('gamepadIndices = Gamepad(''GetGamepadNumbersFromNames'', gamepadNames)\n');
            fprintf('\n');
            fprintf('Given string or cell array of strings holding gamepad names return a vector\n');
            fprintf('of the indices of all gameads with those names.   The gamepad name is the same as the\n');
            fprintf('product field of the HID device structure returned by PsychHID, the name of the device \n');
            fprintf('assigned by the manufacturer; Identical joysticks should report\n');
            fprintf('the same device name.\n');
            fprintf('\n');
            return
        case{'GETNUMBUTTONS?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('numButtons = Gamepad(''GetNumButtons'', gamepadIndex)\n');
            fprintf('\n');
            fprintf('Given a gamepad number return the number of buttons on the gamepad.\n');
            fprintf('Gamepad(''GetButton'' will return the state of any button.\n');
            fprintf('\n');
            return
        case{'GETNUMAXES?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('\n');
            fprintf('numAxes = Gamepad(''GetNumAxes'', gamepadIndex)\n');
            fprintf('Given a gamepad index return the number of Axes on the gamepad.\n');
            fprintf('Gamepad(''GetAxis'') will return the state of any axis.\n');
            fprintf('\n');
            return
        case{'GETNUMBALLS?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('\n');
            fprintf('numBalls = Gamepad(''GetNumBalls'', gamepadIndex)\n');
            fprintf('Given a gamepad index return the number of balls on the gamepad.\n');
            fprintf('Gamepad(''GetBall'') will return the state of any ball.\n');
            fprintf('\n');
            return
        case{'GETNUMHATS?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('numHats = Gamepad(''GetNumHats'', gamepadIndex)\n');
            fprintf('\n');
            fprintf('Given a gamepad index return the number of hats on the gamepad.\n');
            fprintf('Gamepad(''GetHat'') will return the state of any hat.\n');
            fprintf('\n');
            return
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
            return
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
            return
        case{'GETAXISRAWMAPPING?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('handles = Gamepad(''GetAxisRawMapping'', gamepadIndex,axisIndex)\n');
            fprintf('\n');
            fprintf('Given a gamepad index and an axis index, returns low-level handles to the \n');
            fprintf('specified axis on the specified gamepad.\n');
            fprintf('You can query the raw state of the axis via rawState = PsychHID(''RawState'', handles(1), handles(2));\n');
            fprintf('on OS/X. On Linux you can call [d1,d2,d3,d4,v] = GetMouse([], handles(1)); rawState = v(handles(2));.\n');
            fprintf('This is significantly faster than calling the ''GetAxis'' function, as it bypasses all error-checking\n');
            fprintf('and remapping of raw device values to the normalized range of (-32768 to 32768).\n\n');
            return
        case{'GETBUTTONRAWMAPPING?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('handles = Gamepad(''GetButtonRawMapping'', gamepadIndex,axisIndex)\n');
            fprintf('\n');
            fprintf('Given a gamepad index and a button index, returns low-level handles to the \n');
            fprintf('specified button on the specified gamepad.\n');
            fprintf('You can query the raw state of the button via rawState = PsychHID(''RawState'', handles(1), handles(2));\n');
            fprintf('on OS/X. On Linux you can call [d1,d2,b] = GetMouse([], handles(1)); rawState = b(handles(2));.\n');
            fprintf('This is significantly faster than calling the ''GetButton'' function, as it bypasses all error-checking\n');
            fprintf('and remapping of raw device values.\n\n');
            return
        case{'GETHATRAWMAPPING?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('handles = Gamepad(''GetHatRawMapping'', gamepadIndex,axisIndex)\n');
            fprintf('\n');
            fprintf('Given a gamepad index and a hat index, returns low-level handles to the \n');
            fprintf('specified hat on the specified gamepad.\n');
            fprintf('You can query the raw state of the hat via rawState = PsychHID(''RawState'', handles(1), handles(2));\n');
            fprintf('This is significantly faster than calling the ''GetHat'' function, as it bypasses all error-checking\n');
            fprintf('and remapping of raw device values.\n\n');
            return
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
            return
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
            fprintf('hatState is an integer in the range 1-9 indicting at which of eight \n');
            fprintf('compass points or center the hat switch is positioned.  see "help Gamepad"\n');
            fprintf('for a list of functions such as "HatUp" which return constants for named\n');
            fprintf('compass points.\n');
            fprintf('\n');
            return
        case{'UNPLUG?'}
            fprintf('Usage:\n');
            fprintf('\n');
            fprintf('Gamepad(''Unplug'')\n');
            fprintf('\n');
            fprintf('Resets the internal state of Gamepad function.  Call before plugging or\n');
            fprintf('unplugging gamepads. Unsupported with GNU/Octave, problematic on Linux!\n');
            fprintf('\n');
            return
    end %switch
else
    % No recognized command at all:
    error('Invalid or unknown/unsupported subcommand. Check for typos!');
end
