function CedrusResponseBoxTest
% Test for proper operation of the Cedrus RB-x30 response pad driver
% "CedrusResponseBox".
%
%

% History:
% 04/26/08 Written (MK).

% Enumerate all potential response box devices:
if IsOSX
    devs = dir('/dev/cu.usbserial*');
end

if IsLinux
    devs = dir('/dev/ttyUSB*');
end

if IsWin
    % Ok, no way of really enumerating what's there, so we simply predifne
    % the first 7 hard-coded port names:
    devs = {'COM0', 'COM1', 'COM2', 'COM3', 'COM4', 'COM5', 'COM6'};
end

if isempty(devs)
    error('No box detected: Box disconnected or driver not properly installed/configured?!?');
end

% Let user choose if there are multiple potential candidates:
if length(devs) > 1
    fprintf('Following devices are potentially Cedrus RB devices: Choose one.\n\n');
    for i=1:length(devs)
        fprintf('%i: %s\n', i, char(devs(i).name));
    end
    fprintf('\n\n');

    while 1
        devidxs = input('Type the number + ENTER of your response pad device: ', 's');
        devid = str2double(devidxs);

        if isempty(devid)
            fprintf('Type the number please, not something else!\n');
        end

        if ~isnumeric(devid)
            fprintf('Type the number please, not something else!\n');
        end

        if devid < 1 || devid > length(devs) || ~isfinite(devid)
            fprintf('Type one of the listed numbers please!\n');
        else
            break;
        end
    end
    
    if ~IsWin
        port = ['/dev/' char(devs(devid).name)];
    else
        port = char(devs(devid).name);
    end
else
    % Only one candidate: Choose it.
    if ~IsWin
        port = ['/dev/' char(devs(1).name)];
    else
        port = char(devs(1).name);
    end
end

fprintf('\n\nThe following (Cedrus serial port) device will be used for testing: %s\n\n', port);

% 'port' contains the port name string of the serial port device file to
% connect to for testing.

% Everything from now on is try-catch protected in case of error:
try
    % Open display on maximum screen with black background color:
    oldverbosity = Screen('Preference', 'Verbosity', 2);
    screenid = max(Screen('Screens'));
%    screenid = 0
    w=Screen('OpenWindow', screenid, 0); %, [0 0 300 300]);

    % Try to open and init the box, return handle 'h' to it:
    h = CedrusResponseBox('Open', port);
    
    % Diplay detected device info:
    fprintf('Device info from box:\n\n');
    devinfo = CedrusResponseBox('GetDeviceInfo', h) %#ok<NOPRT,NASGU>
    
    fprintf('Device name and vendor: %s.\n\n', devinfo.Name);
    
    % Flush the box for a start:
    CedrusResponseBox('FlushEvents', h);
    
    
    % External trigger test:
    fprintf('Configuring RJ-45 connector for TTL photo-diode input on pin 0:\n');
    
    % Switch RJ-45 connector to general purpose I/O mode:
    fprintf('Old connector mode: %s\n', CedrusResponseBox('SetConnectorMode', h));
    CedrusResponseBox('SetConnectorMode', h, 'GeneralPurpose');
    fprintf('New connector mode: %s\n', CedrusResponseBox('SetConnectorMode', h));
    
    % Set all 6 pins (0 to 5) as input pins which trigger on TTL high (1)
    CedrusResponseBox('DefineInputLinesAndLevels', h, [4], 1, 50);
            
    % Ok start of pseudo-trial: Reset box reaction time timer. 'basetime'
    % is our best GetSecs estimate of when timer started with value zero:
    basetime = CedrusResponseBox('ResetRTTimer', h);
    lasttime = basetime;
    
    % This may have caused a few spurious events on photo-diode, therefore
    % we flush the event queue again after a second:
    KbStrokeWait;
    WaitSecs(1);
    CedrusResponseBox('FlushEvents', h);

    % Execute 10 measurement trials:
    for trial=1:10
        % Draw all-white stimulus in top-left 100 x 100 pixels square of screen:
        Screen('FillRect', w, 255, [0 0 100 100]);

        % Stimulus onset shall be about 2 seconds after 'basetime', rounded to
        % next vertical retrace:
        [vbl, visonset] = Screen('Flip', w, lasttime + 2);

        % Ok, the delta between 'visonset' and 'basetime' should be the RTT
        % value when the box detected a trigger signal from the photo-diode,
        % assuming diode was mounted in top-left corner of display and display
        % is sufficiently fast, ie. a CRT display:
        expectedDelta = visonset - basetime;

        % Wait for the box to return an event:
        while 1
%            evt = CedrusResponseBox('WaitButtonPress', h);
            evt = CedrusResponseBox('GetButtons', h);
            evt %#ok<NOPRT>
            if ~isempty(evt) && evt.button == 6 && evt.action == 1
                break;
            end
        end

        % Extract real delta:
        realDelta = evt.rawtime;

        fprintf('Trial %i: Expected onset timestamp %f secs, real timestamp %f secs -> Delta %f secs.\n', trial, expectedDelta, realDelta, realDelta - expectedDelta);

        % Clear screen back to black:
        lasttime = Screen('Flip', w, visonset + 1);
        WaitSecs(0.5);
        CedrusResponseBox('FlushEvents', h)
        % Next trial:
    end

%     KbReleaseWait
%     while ~KbCheck
%         evt = CedrusResponseBox('WaitButtons', h)
%     end
    
    % Done with test: Close screen and box:
    Screen('CloseAll');
    CedrusResponseBox('CloseAll');
    
    fprintf('Test finished, bye!\n\n');
    
catch
    % In case of error, close box and display:
    Screen('CloseAll');
    CedrusResponseBox('CloseAll');
    
    psychrethrow(psychlasterror);
end

if exist('oldverbosity', 'var')
    Screen('Preference', 'Verbosity', oldverbosity);
end
