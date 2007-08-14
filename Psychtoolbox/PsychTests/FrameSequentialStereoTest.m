function FrameSequentialStereoTest(screenid)
% FrameSequentialStereoTest(screenid)
%
% Tests presentation order and timing of stimulus onset in frame sequential
% stereo mode. This is a perceptual test.
%
% The test will run on the screen with the highest number by default, but
% you can pass a 'screenid' to it, if you want.
%
% After startup, the display will show a mostly constant yellow, red or
% green display. Depending on what you see, it tells you how your graphics
% hardware syncs buffer onset:
%
% A) If you see a constant yellow screen, then your hardware always makes sure
% that a full stereo cycle is presented before executing a Screen('flip')
% command, however its not defined if onset of a new stimulus will happen
% with the left buffer first or with the right buffer. You'd need
% measurement equipment to verify which is the startbuffer. The granularity
% of buffer swaps will be reduced to the duration of two video refresh
% intervals, as indicated by the 'ifi' number printed to the Matlab window.
%
% B) If you see either a constant green or a constant red at each run of the
% script (always red or always green, never anything else), then your
% hardware will sync bufferswaps always to the onset of the left stereo
% buffer (if you see red) or to the right buffer (if you see green).
%
% C) If you see either a constant green or a constant red during one run of the
% script, but you observe different colors at repeated runs of the script,
% or a slow switching between red and green, then that means that your
% hardware doesn't care about syncing bufferswaps to a fixed buffer,
% instead it just swaps at the next VBL, so either the left- or the right-
% stereo buffer of a new stimulus will show first. The maximum flip rate is
% equal to the monitor refresh rate.
%
% In case C) you are out of luck if you use Microsoft Windows. If you use
% Linux, then contact the forum with a feature request, we may implement a
% solution for you. If you run MacOS/X, you can 'force' the graphics
% hardware to always synchronize stimulus onset (flip) to either the left-
% or the right buffers display cycle, and you can query which buffer was
% displayed first after flip, left- or right. However this is a software
% trick which needs calibration. To test if the trick works on your setup,
% do the following:
%
% 1. Wait until color of the display settles to either red or green.
% 2. If its green, press the 'g' key, if its red, press the 'r' key.
% 3. After a short period, the display should settle to a nice green,
% indicating that the trick works, and PTB should write 'GREEN' to the
% Matlab window.
% 4. If the display doesn't stabilize to green, but at least the output of
% the Matlab window corresponds with your percept (GREEN for green, RED for
% red) then that means that your system has very noisy timing.

% History:
% 08/14/07 Written (MK).

% Running on PTB-3?
AssertOpenGL;

% Same keymappings for all platforms:
KbName('UnifyKeyNames');

redfield = -1;

if nargin < 1
    screenid = [];
end

if isempty(screenid)
    screenid = max(Screen('Screens'));
end

if IsOSX==0
    fprintf('Caution: While the perceptual test in this script will still work (perceptually),\n');
    fprintf('query of the stereobuffer presentation with which flip was synchronized and control\n');
    fprintf('of target stereobuffer to which flip should synchronize, will not work on your operating\n');
    fprintf('system. This feature is only available on Macintosh OS/X systems, and scheduled for inclusion\n');
    fprintf('on GNU/Linux systems. No support for MS-Windows is planned, due to limitations of that OS.\n\n');
end

% Open double-buffered frame-sequential stereo window on display
% 'screenid', background clear color is black (==0):
win = Screen('OpenWindow', screenid, 0, [], [], [], 1);
ifi = Screen('GetFlipInterval', win)

% Wait until all keys are released:
while KbCheck; end;
count = 0;
delta=[];

% Initialize target field to zero. This is purely random coice and will get
% properly set, once we know the real assignment of buffers to fields.
targetfield = -1;

% Run test loop until keypress:
while 1
    % Count iterations:
    count = count + 1;
    % Show both stereo buffers in black:
    Screen('SelectStereoDrawbuffer', win, 0, -1);
    Screen('FillRect', win, 0);
    Screen('SelectStereoDrawbuffer', win, 1);
    Screen('FillRect', win, 0);
    
    % Flip at earliest point in time:
    t1 = Screen('Flip', win);
    
    % Show left stereo buffer in red:
    Screen('SelectStereoDrawbuffer', win, 0, targetfield);
    Screen('FillRect', win, [255 0 0]);
    % Show right stereo buffer in green:
    Screen('SelectStereoDrawbuffer', win, 1);
    Screen('FillRect', win, [0 255 0]);
    
    % Flip at earliest point in time:
    t2 = Screen('Flip', win);
    
    % Query VBL count of the video frame corresponding to last flip:
    winfo = Screen('GetWindowInfo', win);
    fieldid = winfo.VBLCount;
    if redfield~=-1
        if mod(fieldid, 2) == redfield
            fprintf('RED\n');
        else
            fprintf('GREEN\n');
        end
        
        % Select target start field for flips to be a green frame. If
        % everything works, then the user should only see green after
        % calibration:
        targetfield = 1-redfield;
    else
        fprintf('UNKNOWN: Press g if screen is green, r if screen is red.\n');
    end
    
    delta(count) = t2 - t1;
    
    [down secs keycode] = KbCheck;
    if down
        if keycode(KbName('ESCAPE'))
            break;
        end
        
        if keycode(KbName('r'))
            redfield = mod(fieldid, 2);
        end
        
        if keycode(KbName('g'))
            redfield = mod(fieldid+1, 2);
        end

        if keycode(KbName('x'))
            while KbCheck; end;
            KbWait;
        end
    end
end

% Close display:
Screen('CloseAll');

med = median(delta)
avg = mean(delta)

% Done.
return;
