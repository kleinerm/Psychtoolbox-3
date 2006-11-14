% DrawMirroredTextDemo
%
% _______________________________________________________
%
%  Trivial example of drawing mirrored text. This demonstrates how to use
%  low-level OpenGL API functions to apply geometric transformations to
%  the drawn objects.
% _________________________________________________________________________
%
% see also: PsychDemos

% 3/8/04    awi     Wrote it.
% 7/13/04   awi     Added comments section.  
% 9/8/04    awi     Added Try/Catch, cosmetic changes to documentation.
% 11/1/05   mk      Derived from DrawSomeTextOSX.

if IsLinux
    error('DrawMirroredTextDemoOSX does not work yet on GNU/Linux.');
end;

text='Hello World! QqPpYyGgJj Press key multiple times to exit.'
x = 100;
y = 100;

try
    % Choosing the display with the highest dislay number is
    % a best guess about where you want the stimulus displayed.
    screens=Screen('Screens');
    screenNumber=max(screens);

    w=Screen('OpenWindow', screenNumber,0,[],32,2);
    Screen('FillRect', w, [0, 0, 0]);
    Screen('TextFont',w, 'Courier New');
    Screen('TextStyle', w, 1);

    % Loop through all text sized from 20 to 80 pixels...
    for mysize=20:5:80
        % Setup text size:
        Screen('TextSize',w, mysize);
        % Draw text 'normal'
        Screen('DrawText', w, text, x, y + 100, [255, 255, 255]);

        % Paint a green frame around it, just to demonstrate the
        % 'TextBounds' command:
        [textbox , textbox2] = Screen('TextBounds', w, text);
        textbox = OffsetRect(textbox, x, y);
        Screen('FrameRect', w, [0, 255, 0, 255], textbox);

        % Setup mirror transformation for horizontal flipping:

        % xc, yc is the geometric center of the text.
        [xc, yc] = RectCenter(textbox);

        % Make a backup copy of the current transformation matrix for later
        % use/restoration of default state:
        Screen('glPushMatrix', w);
        % Translate origin into the geometric center of text:
        Screen('glTranslate', w, xc, 0, 0);
        % Apple a scaling transform which flips the diretion of x-Axis,
        % thereby mirroring the drawn text horizontally:
        Screen('glScale', w, -1, 1, 1);
        % We need to undo the translations...
        Screen('glTranslate', w, -xc, 0, 0);
        % The transformation is ready for mirrored drawing of text:
        
        % Draw text again, this time mirrored as it is affected by the
        % mirror transform that has been setup above:
        Screen('DrawText', w, text, x, y, [255, 255, 0]);

        % Restore to non-mirror mode, aka the default transformation
        % that you've stored on the matrix stack:
        Screen('glPopMatrix', w);

        % Now all transformations are back to normal and we can proceed
        % ordinarily...
        
        % Flip the display:
        Screen('Flip',w);

        % Wait for keypress:
        while KbCheck
        end;
        KbWait;
        % Next iteration...
    end;

    % Done!
    Screen('CloseAll');
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end % try..catch



