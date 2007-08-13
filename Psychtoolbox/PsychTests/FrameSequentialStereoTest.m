function FrameSequentialStereoTest(screenid)
% FrameSequentialStereoTest
%
% Tests presentation order and timing of stimulus onset in frame sequential
% stereo mode. This is a perceptual test.

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

% Open double-buffered frame-sequential stereo window on display
% 'screenid', background clear color is black (==0):
win = Screen('OpenWindow', screenid, 0, [], [], [], 1);
ifi = Screen('GetFlipInterval', win)

% Wait until all keys are released:
while KbCheck; end;
count = 0;
delta=[];

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
    Screen('SelectStereoDrawbuffer', win, 0, 0);
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
