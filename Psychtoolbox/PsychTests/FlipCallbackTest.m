function FlipCallbackTest()

    PsychDefaultSetup(2);
    screen = max(Screen('Screens'));
    screensize=Screen('Rect', screen);
    screenheight = screensize(4);
    winheight = round(screenheight/4);

    win1 = Screen('OpenWindow', screen, [255 255 0], [0 0 winheight winheight]);
    win2 = Screen('OpenWindow', screen, [0 255 255], [winheight 0 2*winheight winheight]);

    % Only the callback from win1 should be executed (and time struct should be ignored)
    Screen('ScheduleCallbackOnFlip', win1, @(~) fprintf('Hello from win1\n'));
    Screen('ScheduleCallbackOnFlip', win2, @(~) fprintf('Hello from win2\n'));

    Screen('Flip', win1);

    Screen('ClearFlipCallbacks', win2);
    Screen('Flip', win2);

    % Access VBL timestamp from the callback
    Screen('ScheduleCallbackOnFlip', win1, @(t) fprintf('Last VBL time for win1: %.4fs\n', t.VBLTimestamp));
    Screen('ScheduleCallbackOnFlip', win2, @(t) fprintf('Last VBL time for win2: %.4fs\n', t.VBLTimestamp));

    Screen('Flip', win1);
    Screen('Flip', win2);

    % Demonstrate passing extra arguments
    Screen('ScheduleCallbackOnFlip', win1, @aFunction, 4, 2);
    Screen('Flip', win1);

    % Demonstrate closing the window implicitly clears pending callbacks
    Screen('ScheduleCallbackOnFlip', win2, @(~) fprintf('This will not happen.\n'));
    Screen('Close', win2);

    win2 = Screen('OpenWindow', screen, [0 255 255], [winheight 0 2*winheight winheight]);
    Screen('Flip', win2);

    % Scheduling a callback captures the arguments by value, so that changing them later has no effect
    x = 5;
    Screen('ScheduleCallbackOnFlip', win2, @(~, a, b) fprintf('A - B = %i (should be 2)\n', a - b), x, 3);
    clear x;
    Screen('Flip', win2);

    Screen('Close', win1);
    Screen('Close', win2);
end

function aFunction(t, arg1, arg2)
    fprintf('The sum of %i and %i is %i at VBL time=%.4fs\n', arg1, arg2, arg1+arg2, t.VBLTimestamp);
end