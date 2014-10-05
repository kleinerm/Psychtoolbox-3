function KbDemo
%% KbDemo
% Shows how to detect when the user has pressed a key.
% See KbCheck, KbName, KbWait, GetChar, CharAvail.
%
% The KbXXX functions are low-level and go after the state of the keyboard.
% The GetChar/CharAvail interface pulls characters out of the event queue.
% The advantage of the KbXXX functions is that if you can sit and wait on
% characters in a tight loop, you'll get less of a lag.  But if the key
% goes up and down in between when your code checks the keyboard state, you'll
% miss it.
% 
% The advantage of GetChar/CharAvail is that they may be used
% asychronously - the OS will pick up the character whether your code
% is looking for it when it comes.
%
% This demo works fine the with Keyspan Digital Media Remote:
% http://www.keyspan.com/products/usb/remote/
% However, the Digital Media Remote is sluggish (max alternating key rate of 2 Hz),  
% suggesting that it may not be well suited for measuring reaction time.
%
% Normally, characters typed creep through and appear in the command window or
% file being edited, depending on which has focus. You can prevent Matlab
% from receiving typed characters by calling ListenChar(2); at the
% beginning of you script and calling ListenChar(0); at the end of your
% script. Caution: Make sure to always call ListenChar(0) at normal exit and in all
% error handling routines, otherwise Matlab will be left with a dead
% keyboard and you'll be unable to type any commands into the Matlab
% window! Pressing CTRL+C will reenable the keyboard in such a case.

% David Brainard, January, 1997
% Allen Ingling, February, 1998

% 01/22/97	dhb Wrote demos 2 & 3, as KeyTimingDemo.
% 03/06/97	dhb	Change from TIMER to modern routines.
% 03/08/97	dgp	Polished the instructions.
% 08/02/97	dgp	Call FlushEvents('keyDown') after getting each
%       key press, to start fresh each time.
%       Explain difference between key presses and characters.
% 01/28/98	dgp	Explain effect of CapsLock key.
% 02/28/98	dgp	Mention new capability of KbCheck.
% 02/18/98	awi	Wrote demos 1 & 4.
% 02/19/98	dgp	Folded in awi's KbExplore.
% 02/28/98	dgp	Folded in dhb's KeyTimingDemo.
% 03/02/98	dgp	Cosmetic.
% 03/09/98	dhb Change case of screen to Screen.
%       Force 8-bit depth. DefaultClut.
% 03/10/98	dgp	Allow all depths. Use new WhiteIndex and BlackIndex.
% 03/12/98	dgp	Make sure Backgrounding is off.
% 03/19/99	dgp	Use KbName. We no longer use GetChar, so Backgrounding no longer matters.
% 06/06/00	dgp	Use arrow and escape keys instead of a, s and q to allow use with
%		the Keyspan Digital Media Remote, which has no letter keys.
%		http://www.keyspan.com/products/usb/remote/
% 02/11/01  awi Added platform conditionals for font names.
%       Conditionally call flushevents only on OS9. 
% 04/03/02  awi Restored flushevents under Windows
% 04/13/02  dgp Use Arial, eliminate conditionals.
% 09/23/06  rhh Ported part four of the demo to Mac OS X.
% 10/11/06  dhb  Imported subfunctions into single file, use cell syntax.
% 10/11/06  dhb Added comment comparing to GetChar/CharAvail.
% 10/16/06  mk  Add call 'UnifyKeyNames' -> Same keynames on all systems.
% 10/17/06  mk  Remove calls to FlushEvents. They are meaningless for KbWait and KbCheck,
%               would only affect CharAvail and GetChar.


% Enable unified mode of KbName, so KbName accepts identical key names on
% all operating systems:
KbName('UnifyKeyNames');

%% Runs 4 demos in sequence
% Report keyboard key number for any pressed keys, including
% modifier keys such as <shift>, <control>, <caps lock> and <option>.  The
% only key not reported is the start key, which turns on the computer.
KbDemoPart1;
WaitSecs(0.5);

% Report time of keypress, using KbCheck.
KbDemoPart2;
WaitSecs(0.5);

% Report time of keypress, using KbWait.
KbDemoPart3;
WaitSecs(0.5);

% Use keys as real-time controls of a dynamic display.
KbDemoPart4;
return

%% Part 1
function KbDemoPart1
% Displays the key number when the user presses a key.

fprintf('1 of 4.  Testing KbCheck and KbName: press a key to see its number.\n');
fprintf('Press the escape key to proceed to the next demo.\n');
escapeKey = KbName('ESCAPE');
while KbCheck; end % Wait until all keys are released.

while 1
    % Check the state of the keyboard.
    [ keyIsDown, seconds, keyCode ] = KbCheck;

    % If the user is pressing a key, then display its code number and name.
    if keyIsDown

        % Note that we use find(keyCode) because keyCode is an array.
        % See 'help KbCheck'
        fprintf('You pressed key %i which is %s\n', find(keyCode), KbName(keyCode));

        if keyCode(escapeKey)
            break;
        end
        
        % If the user holds down a key, KbCheck will report multiple events.
        % To condense multiple 'keyDown' events into a single event, we wait until all
        % keys have been released.
        KbReleaseWait;
    end
end

return

%% Part 2
function KbDemoPart2
% Displays the number of seconds that have elapsed when the user presses a
% key.
fprintf('\n2 of 4. Testing KbCheck timing: please type a few keys.  (Try shift keys too.)\n');
fprintf('Type the escape key to proceed to the next demo.\n');
escapeKey = KbName('ESCAPE');
startSecs = GetSecs;

while 1
    [ keyIsDown, timeSecs, keyCode ] = KbCheck;
    if keyIsDown
        fprintf('"%s" typed at time %.3f seconds\n', KbName(keyCode), timeSecs - startSecs);
        if keyCode(escapeKey)
            break;
        end

        % If the user holds down a key, KbCheck will report multiple events.
        % To condense multiple 'keyDown' events into a single event, we wait until all
        % keys have been released.
        KbReleaseWait;
    end
end

return

%% Part 3
function KbDemoPart3
% Wait for a key with KbWait.
fprintf('\n3 of 4.  Testing KbWait: hit any key.  Just once.\n');
startSecs = GetSecs;
timeSecs = KbWait;
[ keyIsDown, t, keyCode ] = KbCheck;
fprintf('"%s" typed at time %.3f seconds\n', KbName(keyCode), timeSecs - startSecs);

return

%% Part 4
function KbDemoPart4
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
    
    % Use the parameters.
    spotDiameter = spotRadius * 2;
    spotRect = [0 0 spotDiameter spotDiameter];
    centeredspotRect = CenterRect(spotRect, windowRect); % Center the spot.
    rotationAngle = initialRotationAngle;
    
    % Set up the timer.
    startTime = now;
    durationInSeconds = 60 * 2;
    numberOfSecondsRemaining = durationInSeconds;
    
    % Loop while there is time.
    while numberOfSecondsRemaining > 0
            numberOfSecondsElapsed = round((now - startTime) * 10 ^ 5);
            numberOfSecondsRemaining = durationInSeconds - numberOfSecondsElapsed;
        
            Screen('DrawText', window, '4 of 4.  Press the left or right arrow key to move, or the escape key to quit.', 20, 20, black);
            Screen('DrawText', window, sprintf('%i seconds remaining...', numberOfSecondsRemaining), 20, 50, black);
            
            xOffset = rotationRadius * cos(rotationAngle);
            yOffset = rotationRadius * sin(rotationAngle);
            offsetCenteredspotRect = OffsetRect(centeredspotRect, xOffset, yOffset);
            Screen('FillOval', window, [0 0 127], offsetCenteredspotRect);
            Screen('Flip', window);
            
            [ keyIsDown, seconds, keyCode ] = KbCheck;
            
            if keyIsDown
                if keyCode(rightKey)
                    if rotationAngle < 2 * pi
                        rotationAngle = rotationAngle + 0.1;
                    else
                        rotationAngle = 0;
                    end
                elseif keyCode(leftKey)
                    if rotationAngle > 0
                        rotationAngle = rotationAngle - 0.1;
                    else
                        rotationAngle = 2 * pi;
                    end
                elseif keyCode(escapeKey)
                    break;
                end
            end
            
    end
    
    Screen('CloseAll');

    fprintf('\n4 of 4.  Done.\n');

catch
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end
return
