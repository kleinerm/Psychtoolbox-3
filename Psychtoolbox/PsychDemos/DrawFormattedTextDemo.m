
% DrawFormattedTextDemo
%
% Draws lots of formatted text, shows how to center text vertically and/or
% horizontally, how line-breaks are introduced, how to compute text
% bounding boxes.
%
% Press any key to cycle through different demo displays.
%
% see also: PsychDemosOSX, Screen DrawText?, DrawSomeTextDemo

% 10/16/06    mk     Wrote it.

try
    % Choosing the display with the highest display number is
    % a best guess about where you want the stimulus displayed.
    screens=Screen('Screens');
    screenNumber=max(screens);

    % Open window with default settings:
    w=Screen('OpenWindow', screenNumber);

    % Select specific text font, style and size, unless we're on Linux
    % where this combo is not available:
    if IsLinux==0
        Screen('TextFont',w, 'Courier New');
        Screen('TextSize',w, 14);
        Screen('TextStyle', w, 1+2);
    end;

    % Read some text file:
    fd = fopen([PsychtoolboxRoot 'Contents.m'], 'rt');
    if fd==-1
        error('Could not open Contents.m file in PTB root folder!');
    end
    
    mytext = '';
    tl = fgets(fd);
    lcount = 0;
    while (tl~=-1) & (lcount < 48)
        mytext = [mytext tl];
        tl = fgets(fd);
        lcount = lcount + 1;
    end
    fclose(fd);
    
    Screen('DrawText', w, 'Top-Left aligned, 40 chars wide: Hit any key to continue.', 0, 0, [255, 0, 0, 255]);

    % mytext contains the content of the first 48 lines of the text file.
    % Let's print it: Start at (x,y)=(10,10), break lines after 40
    % characters:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 10, 20, 0, 40);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('Flip',w);
    KbWait;
    while KbCheck; end;
    
    % Draw text again, this time with unlimited line length:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 10, 10, 0);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Top-Left aligned, width unconstrained: Hit any key to continue.', nx, ny, [255, 0, 0, 255]);
    Screen('Flip',w);
    KbWait;
    while KbCheck; end;

    % Now horizontally and vertically centered:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 'center', 'center', 0);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Centered: Hit any key to continue.', nx, ny, [255, 0, 0, 255]);
    Screen('Flip',w);
    KbWait;
    while KbCheck; end;

    % Now vertically centered:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 10, 'center', 0);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Vertically centered: Hit any key to continue.', nx, ny, [255, 0, 0, 255]);
    Screen('Flip',w);
    KbWait;
    while KbCheck; end;

    % End of demo, close window:
    Screen('CloseAll');
catch
    % This "catch" section executes in case of an error in the "try"
    % section []
    % above.  Importantly, it closes the onscreen window if it's open.
    Screen('CloseAll');
    fclose('all');
    psychrethrow(psychlasterror);
end
