% DrawFormattedTextDemo
%
% Draws lots of formatted text, shows how to center text vertically and/or
% horizontally, how line-breaks are introduced, how to compute text
% bounding boxes.
%
% Press any key to cycle through different demo displays.
%
% see also: PsychDemos, Screen DrawText?, DrawSomeTextDemo

% 10/16/06    mk     Wrote it.

try
    % Choosing the display with the highest display number is
    % a best guess about where you want the stimulus displayed.
    screens=Screen('Screens');
    screenNumber=max(screens);

    % Open window with default settings:
    w=Screen('OpenWindow', screenNumber);

    % Select specific text font, style and size:
    Screen('TextFont',w, 'Courier New');
    Screen('TextSize',w, 14);
    Screen('TextStyle', w, 1+2);

    % Read some text file:
    fd = fopen([PsychtoolboxRoot 'Contents.m'], 'rt');
    if fd==-1
        error('Could not open Contents.m file in PTB root folder!');
    end
    
    mytext = '';
    tl = fgets(fd);
    lcount = 0;
    while lcount < 48
        mytext = [mytext tl]; %#ok<*AGROW>
        tl = fgets(fd);
        lcount = lcount + 1;
    end
    fclose(fd);
    mytext = [mytext char(10)];
    
    % Get rid of '% ' symbols at the start of each line:
    mytext = strrep(mytext, '% ', '');
    mytext = strrep(mytext, '%', '');

    Screen('DrawText', w, 'Top-Left aligned, max 40 chars wide: Hit any key to continue.', 0, 0, [255, 0, 0, 255]);

    % mytext contains the content of the first 48 lines of the text file.
    % Let's print it: Start at (x,y)=(10,10), break lines after 40
    % characters:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 10, 20, 0, 40);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('Flip',w);
    KbStrokeWait;

    Screen('DrawText', w, 'Top-Left aligned, max 60 chars wide, justified: Hit any key to continue.', 0, 0, [255, 0, 0, 255]);
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 'wrapat', 40, 0, 60);
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('Flip',w);
    KbStrokeWait;

    % Use Left- and right justified text. Specify the x-start (left border) position as part of
    % the optional 11'th 'winrect' parameter [50 40 100 100], as we obviously can't specify it
    % as 'sx' parameter - that one being taken by 'justifytomax'. Only the left border value 50
    % is taken from the given winrect, the other right/bottom/top parameters are ignored:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 'justifytomax', 40, 0, [], [], [], [], [], [50 40 100 100]);
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Left- and right justified, width unconstrained: Hit any key to continue.', nx, ny, [255, 0, 0, 255]);

    Screen('Flip',w);
    KbStrokeWait;

    % Draw text again, this time with unlimited line length.
    % Also request individual bounding boxes for each word in the text.
    [nx, ny, bbox, wordboxes] = DrawFormattedText(w, mytext, 10, 10, [0 0 255]);
    
    % Show a bounding box for each word, slightly enlarged for clarity:
    Screen('FrameRect', w, [0 255 0], GrowRect(wordboxes, 2, 2)');

    % Show computed global text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Top-Left aligned, width unconstrained, per word bounding boxes: Hit any key to continue.', nx, ny, [255, 0, 0, 255]);
    Screen('Flip',w);
    KbStrokeWait;

    % Draw text again, this time with unlimited line length and
    % right-aligned, and with reversed text direction right-to-left:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 'right', 10, 0, [], [], [], [], 1, bbox);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Top-Right aligned, reversed text direction, width unconstrained: Hit any key to continue.', bbox(RectLeft), ny, [255, 0, 0, 255]);
    Screen('Flip',w);
    KbStrokeWait;
    
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 'right', 10, 0, [], [], [], [], 0, bbox);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Top-Right aligned, width unconstrained: Hit any key to continue.', bbox(RectLeft), ny, [255, 0, 0, 255]);
    Screen('Flip',w);
    KbStrokeWait;
    
    % Now horizontally and vertically centered:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 'center', 'center', 0);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Centered: Hit any key to continue.', nx, ny, [255, 0, 0, 255]);
    Screen('Flip',w);
    KbStrokeWait;

    % Now vertically centered:
    [nx, ny, bbox] = DrawFormattedText(w, mytext, 10, 'center', 0);
    
    % Show computed text bounding box:
    Screen('FrameRect', w, 0, bbox);

    Screen('DrawText', w, 'Vertically centered: Hit any key to continue.', nx, ny, [255, 0, 0, 255]);
    Screen('Flip',w);
    KbStrokeWait;

    Screen('TextSize',w, 22);    
    winHeight = RectHeight(Screen('Rect', w));
    longtext = ['\n\nTeleprompter test: Press any key to continue.\n\n' mytext];
    longtext = repmat(longtext, 1, 3);
    
    tp=zeros(1, 2*winHeight + 1);
    sc = 0;
    
    % Render once, requesting the 'bbox' bounding box of the whole text.
    % This will disable clipping and be very sloooow, so we do it only once
    % to get the bounding box, and later just recycle that box:
    [nx, ny, bbox] = DrawFormattedText(w, longtext, 10, 0, 0);
    Screen('FillRect', w, WhiteIndex(w));
    textHeight = RectHeight(bbox);
    
    for yp = winHeight:-1:-textHeight
        % Draw text again, this time with unlimited line length:
        [nx, ny] = DrawFormattedText(w, longtext, 10, yp, 0);
        Screen('FrameRect', w, 0, bbox);

        sc = sc + 1;
        tp(sc) = Screen('Flip', w);

        if KbCheck
            break;
        end
    end
    
    tp = tp(1:sc);
    fprintf('\nAverage redraw duration for scrolling in msecs: %f\n', 1000 * mean(diff(tp)));
    if 0
        close all;
        plot(1000 * diff(tp));
        title('Redraw duration per scroll frame [msecs]:');
    end
    
    % End of demo, close window:
    sca;
catch %#ok<*CTCH>
    % This "catch" section executes in case of an error in the "try"
    % section []
    % above.  Importantly, it closes the onscreen window if it's open.
    sca;
    fclose('all');
    psychrethrow(psychlasterror);
end
