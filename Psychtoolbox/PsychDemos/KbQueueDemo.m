function KbQueueDemo(deviceIndex)
%% KbQueueDemo([deviceIndex])
% Shows how to detect when the user has pressed a key.
% See KbQueueCheck, KbQueueWait, KbName, KbCheck, KbWait, GetChar, CharAvail.
%
% The KbQueueXXX functions are low-level like KbCheck and KbWait, but, like
% GetChar/CharAvail, they use a queue so that brief events can be captured.
% 
% Like GetChar/CharAvail, KbQueueXXX functions may be used
% asychronously - the OS will pick up the character whether your code
% is currently looking for it or not so long as the queue has already been 
% created(using KbQueueCreate) and started (using KbQueueStart).
%
% Unlike GetChar/CharAvail, KbQueueXXX functions can detect isolated presses
% of modifier keys. Also, the times of key presses should be more accurate than
% those associated with GetChar/CharAvail or with KbCheck and the timebase is
% the same as that returned by GetSecs (unlike GetChar/CharAvail).
%
% The first four demos here are analogous to those in KbDemo.m

% Roger Woods, November, 2007

% 11/03/07  rpw Wrote demos 1-5
% 05/21/12  mk  Add event buffer test to demo 5.
% 01/31/16  mk  Suppress keypress spilling via ListenChar(-1);
% 08/30/16  dcn Add exercising all options of KbQueueWait

if nargin < 1
  deviceIndex = [];
end

% Enable unified mode of KbName, so KbName accepts identical key names on
% all operating systems:
KbName('UnifyKeyNames');

% Prevent spilling of keystrokes into console:
ListenChar(-1);

%% Runs 5 demos in sequence

try
  % Report keyboard key number for any pressed keys, including
  % modifier keys such as <shift>, <control>, <caps lock> and <option>.  The
  % only key not reported is the start key, which turns on the computer.
  KbQueueDemoPart1(deviceIndex);
  WaitSecs(0.5);

  % Report time of keypress, using KbQueueCheck.
  KbQueueDemoPart2(deviceIndex);
  WaitSecs(0.5);

  % Report time of keypress, using KbQueueWait.
  KbQueueDemoPart3(deviceIndex);
  WaitSecs(0.5);

  % Use keys as real-time controls of a dynamic display.
  KbQueueDemoPart4(deviceIndex);

  % Identify keys pressed while other code is executing
  KbQueueDemoPart5(deviceIndex);
  WaitSecs(0.5);

  % Exercise all options of KbQueueWait
  KbQueueDemoPart6(deviceIndex);

  ListenChar(0);
catch
  ListenChar(0);
  psychrethrow(psychlasterror);
end

return

%% Part 1
function KbQueueDemoPart1(deviceIndex)
% Displays the key number when the user presses a key.

fprintf('1 of 6.  Testing KbQueueCheck and KbName: press a key to see its number.\n');
fprintf('Press the escape key to proceed to the next demo.\n');
escapeKey = KbName('ESCAPE');
KbQueueCreate(deviceIndex);
while KbCheck; end % Wait until all keys are released.

KbQueueStart(deviceIndex);

while 1
    % Check the queue for key presses.
    [ pressed, firstPress]=KbQueueCheck(deviceIndex);

    % If the user has pressed a key, then display its code number and name.
    if pressed

        % Note that we use find(firstPress) because firstPress is an array with
        % zero values for unpressed keys and non-zero values for pressed keys
        %
        % The fprintf statement implicitly assumes that only one key will have
        % been pressed. If this assumption is not correct, an error will result

        fprintf('You pressed key %i which is %s\n', min(find(firstPress)), KbName(min(find(firstPress))));

        if firstPress(escapeKey)
            break;
        end
    end
end
KbQueueRelease(deviceIndex);
return

%% Part 2
function KbQueueDemoPart2(deviceIndex)
% Displays the number of seconds that have elapsed when the user presses a
% key.
fprintf('\n2 of 6. Testing KbQueueCheck timing: please type a few keys.  (Try shift keys too.)\n');
fprintf('Type the escape key to proceed to the next demo.\n');
escapeKey = KbName('ESCAPE');
startSecs = GetSecs;

KbQueueCreate(deviceIndex);
KbQueueStart(deviceIndex);

while 1
    [ pressed, firstPress]=KbQueueCheck(deviceIndex);
    timeSecs = firstPress(find(firstPress)); %#ok<FNDSB>
    if pressed
        % Again, fprintf will give an error if multiple keys have been pressed
        fprintf('"%s" typed at time %.3f seconds\n', KbName(min(find(firstPress))), timeSecs - startSecs);
        
        if firstPress(escapeKey)
            break;
        end
	end
end
KbQueueRelease(deviceIndex);
return

%% Part 3
function KbQueueDemoPart3(deviceIndex)

% Wait for the "a" key with KbQueueWait.
keysOfInterest=zeros(1,256);
keysOfInterest(KbName('a'))=1;
KbQueueCreate(deviceIndex, keysOfInterest);
fprintf('\n3 of 6.  Testing KbQueueWait: waiting for a press of the "a" key; all others will be ignored\n');
startSecs = GetSecs;
KbQueueStart(deviceIndex);
timeSecs = KbQueueWait(deviceIndex);

fprintf('The "a" key was pressed at time %.3f seconds\n', timeSecs - startSecs);
KbQueueRelease(deviceIndex);
return

%% Part 4
function KbQueueDemoPart4(deviceIndex)
% Control a screen spot with the keyboard.

% Here are the parameters for this demo.
spotRadius = 25; % The radius of the spot.
rotationRadius = 200; % The radius of the rotation.
initialRotationAngle = 3 * pi / 2; % The initial rotation angle in radians.

try

    % Removes the blue screen flash and minimize extraneous warnings.
    Screen('Preference', 'VisualDebugLevel', 3);
    Screen('Preference', 'SuppressAllWarnings', 1);

    % Find out how many screens and use largest screen number.
    whichScreen = max(Screen('Screens'));
    
    % Open a new window.
    [ window, windowRect ] = Screen('OpenWindow', whichScreen);
    
    % Set text display options. We skip on Linux.
    if ~IsLinux
        Screen('TextFont', window, 'Arial');
        Screen('TextSize', window, 18);
    end

    % Set colors.
    black = BlackIndex(window);
    
    % Set keys.
    rightKey = KbName('RightArrow');
    leftKey = KbName('LeftArrow');
    escapeKey = KbName('ESCAPE');
    
    keysOfInterest=zeros(1,256);
    keysOfInterest(rightKey)=1;
    keysOfInterest(leftKey)=1;
    keysOfInterest(escapeKey)=1;
    
    % Use the parameters.
    spotDiameter = spotRadius * 2;
    spotRect = [0 0 spotDiameter spotDiameter];
    centeredspotRect = CenterRect(spotRect, windowRect); % Center the spot.
    rotationAngle = initialRotationAngle;
    
    % Set up the timer.
    startTime = now;
    durationInSeconds = 60 * 2;
    numberOfSecondsRemaining = durationInSeconds;
    
    KbQueueCreate(deviceIndex, keysOfInterest);
    KbQueueStart(deviceIndex);
    
    % Loop while there is time.
    while numberOfSecondsRemaining > 0
            numberOfSecondsElapsed = round((now - startTime) * 10 ^ 5);
            numberOfSecondsRemaining = durationInSeconds - numberOfSecondsElapsed;
        
            Screen('DrawText', window, '4 of 5.  Press the left or right arrow key to move, or the escape key to quit.', 20,  20, black);
            Screen('DrawText', window, sprintf('%i seconds remaining...', numberOfSecondsRemaining), 20, 50, black);
            
            xOffset = rotationRadius * cos(rotationAngle);
            yOffset = rotationRadius * sin(rotationAngle);
            offsetCenteredspotRect = OffsetRect(centeredspotRect, xOffset, yOffset);
            Screen('FillOval', window, [0 0 127], offsetCenteredspotRect);
            Screen('Flip', window);
            
            % Note that holding down the arrow key without releasing it will
            % not cause he dot to keep moving since this generates only a
            % single key press event (compare to comparable demo in KbDemo.m)
            %
            % We could work around this by making note of whether the key
            % has been released using the call:
            % [pressed, firstPress, firstRelease, lastPress, lastRelease]= KbQueueCheck(deviceIndex)
            % and noting whether lastRelease is more recent than lastPress for
            % the keys of interest, tracking the status across loop iterations.
            % However,it would be easier to use KbCheck, which reflects the
            % current status of the key directly (see KbDemo.m)
            
            [ pressed, firstPress]=KbQueueCheck(deviceIndex);
           
            if pressed
                if firstPress(rightKey)
                    if rotationAngle < 2 * pi
                        rotationAngle = rotationAngle + 0.1;
                    else
                        rotationAngle = 0;
                    end
                elseif firstPress(leftKey)
                    if rotationAngle > 0
                        rotationAngle = rotationAngle - 0.1;
                    else
                        rotationAngle = 2 * pi;
                    end
                elseif firstPress(escapeKey)
                    break;
                end
            end
            
    end
    sca;
    KbQueueRelease(deviceIndex);  % Note that KbQueueRelease is also in the catch clause
    fprintf('\n4 of 6.  Done.\n');    

catch
    KbQueueRelease(deviceIndex);
    sca;
    psychrethrow(psychlasterror);
end
return


%% Part 5
function KbQueueDemoPart5(deviceIndex)
% Prints a list of keys that were pressed while other code was executing

fprintf('5 of 6.  Testing KbQueueCheck asynchronously:\nPress one or more keys during the next 10 seconds.\n');
KbQueueCreate(deviceIndex);
KbQueueStart(deviceIndex);
WaitSecs(10);
fprintf('\n');
KbQueueStop(deviceIndex);	% Stop delivering events to the queue
fprintf('Keypresses during the next 5 seconds will be ignored\n\n');
WaitSecs(5);
[ pressed, firstPress]=KbQueueCheck(deviceIndex);
fprintf('You pressed the following keys during the interval when keypresses were being recorded:\n');
pressedKeys = KbName(firstPress) %#ok<NOPRT,NASGU>

fprintf('\n\nTesting KbEvent-Buffer. It should contain the same information.\n');
fprintf('During the test interval, %i events were recorded.\n', KbEventAvail(deviceIndex));
fprintf('Will print all of them:\n\n');
while KbEventAvail(deviceIndex)
    [evt, n] = KbEventGet(deviceIndex);
    fprintf('Event is:\n'); disp(evt);
    fprintf('\nNow %i events remaining.\n', n);
end

fprintf('Done. Flushing the buffer for fun...\n');
n = KbEventFlush(deviceIndex);
if n == 0
    fprintf('There were zero events remaining at flush time, as expected.\n');
else
    fprintf('Ohoh! There were %i events remaining, while i expected zero events?!?\n', n);
end
fprintf('KbEventBuffer test finished.\n\n');

KbQueueRelease(deviceIndex);
return


%% Part 6
function KbQueueDemoPart6(deviceIndex)
% Exercise all options of KbQueueWait

fprintf('6 of 6.  Testing KbQueueWait''s various wait modes.\n');
% waiting for all keys
KbQueueCreate(deviceIndex);
KbQueueStart(deviceIndex);
startSecs = GetSecs;

% wait for keyup
fprintf('Soon we''ll wait for a key release. Press any key within the next 5 seconds\n');
WaitSecs(5);
fprintf('waiting for key release, release the key you pressed\n');
timeSecs = KbQueueWait(deviceIndex,1);
fprintf('key released at time %.3f seconds\n\n', timeSecs - startSecs);

% wait for keydown
fprintf('Waiting for key press on the keyboard. Press and hold any key\n');
timeSecs = KbQueueWait(deviceIndex,0);
fprintf('key pressed at time %.3f seconds\n\n', timeSecs - startSecs);

% wait for all keys released and then for keydown
fprintf('Waiting for key press, but ignoring any keys that are already down.\nRelease the pressed key now and press another key\n');
timeSecs = KbQueueWait(deviceIndex,2);
fprintf('key pressed at time %.3f seconds\n\n', timeSecs - startSecs);

% wait for keystroke: wait for all keys released and then wait for a
% keydown->key up sequence. Reported time is of the key down, not of the
% key up event that triggers this mode to return
fprintf('Waiting for key stroke, ignoring any keys that are already down.\nRelease the pressed key now and press another key and then release it again\n');
timeSecs = KbQueueWait(deviceIndex,3);
fprintf('key pressed at time %.3f seconds (and released again later)\n', timeSecs - startSecs);

KbQueueRelease(deviceIndex);
return
