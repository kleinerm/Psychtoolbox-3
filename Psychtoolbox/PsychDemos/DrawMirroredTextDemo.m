function DrawMirroredTextDemo(upsideDown)
% DrawMirroredTextDemo([upsideDown=0])
% _______________________________________________________
%
%  Trivial example of drawing mirrored text. This demonstrates how to use
%  low-level OpenGL API functions to apply geometric transformations to
%  the drawn objects.
%
%  Will draw a text string, once in normal orientation, and once mirrored.
%  The mirrored text is either mirrored left<->right, or upside down if you
%  set the optional 'upsideDown' flag to 1. At each key press, the text
%  will be redrawn at an increasing size, until after a couple of redraws
%  the demo will end.
%
%  The demo also draws a bounding box around the text string to demonstrate
%  how you can find out about the bounds of a text string via
%  Screen('TextBounds').
%
%  The mirroring is implemented by first defining a geomtric transformation
%  which will apply to all further drawn shapes and text strings. Then the
%  text string is drawn via Screen('DrawText') -- thereby affected by the
%  geometric transform. Then the transform is undone to "normal".
%
%  How does this work?
%
%  1. The command Screen('glPushMatrix'); makes a "backup copy" of the
%  current transformation state -- the default way of drawing.
%
%  2. The command Screen('glTranslate', w, xc, yc, 0); translates the
%  origin of the coordinate system (which is normally located at the
%  upper-left corner of the screen) into the geometric center of the
%  location of the text string. We find the center xc,yc by retrieving the
%  bounding box of the text string Screen('TextBounds'), then calculating
%  the center of that box [xc, yc].
%
%  3. The Screen('glScale', w, x, y, z); will scale all further drawn
%  objects by a factor 'x' in x-direction (horizontal), 'y' in y-direction
%  (vertical), 'z' in z-direction (depths). Scaling happen with respect to
%  the current origin. As we just set the origin to be the center of the
%  text string in step 2, the object will "scale" around that point. A
%  value of -1 effectively switches the direction along the 'x' axis for a
%  horizontal flip, or along the 'y' axis for an upside down vertical flip.
%  Values with a magnitude other than 1 would scale the whole text up or
%  down in size.
%
%  4. The command Screen('glTranslate', w, -xc, -yc, 0); translates the
%  origin of the coordinate system back to the upper-left corner of the
%  screen. This to make sure that all coordinates provided later on are
%  wrt. to the usual reference frame.
%
%  Steps 2,3 and 4 are internally merged to one mathematical transformation:
%  The flipping of all drawn shapes and objects around the screen position
%  [xc,yc] -- the center of the text string.
%
%  5. We Screen('DrawText') the text string --> The flipping applies.
%
%  6. We undo the whole transformation via Screen('glPopMatrix') thereby
%  restoring the original "do nothing" transformation from the backup copy
%  which was created in step 1. All further drawing is therbey unaffected
%  by the flipping, so we can draw the second copy of the text and the
%  bounding box.
%  
% Besides the scaling and translation transform for moving, rescaling and
% flipping drawn objects and shapes, there is also the Screen('glRotate')
% transform to apply rotation around axis.
%
% This transformations apply to any drawing command, not only text strings!
% _________________________________________________________________________
%
% see also: PsychDemos

% 3/8/04    awi     Wrote it.
% 7/13/04   awi     Added comments section.  
% 9/8/04    awi     Added Try/Catch, cosmetic changes to documentation.
% 11/1/05   mk      Derived from DrawSomeTextOSX.

if nargin < 1
    upsideDown = [];
end

if isempty(upsideDown)
    upsideDown = 0;
end

text = 'Hello World! QqPpYyGgJj Press key multiple times to exit.';
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
        textbox = Screen('TextBounds', w, text);
        textbox = OffsetRect(textbox, x, y);
        Screen('FrameRect', w, [0, 255, 0, 255], textbox);

        % Setup mirror transformation for horizontal flipping:

        % xc, yc is the geometric center of the text.
        [xc, yc] = RectCenter(textbox);

        % Make a backup copy of the current transformation matrix for later
        % use/restoration of default state:
        Screen('glPushMatrix', w);

        % Translate origin into the geometric center of text:
        Screen('glTranslate', w, xc, yc, 0);

        % Apple a scaling transform which flips the diretion of x-Axis,
        % thereby mirroring the drawn text horizontally:
        if upsideDown
            Screen('glScale', w, 1, -1, 1);
        else
            Screen('glScale', w, -1, 1, 1);
        end
        
        % We need to undo the translations...
        Screen('glTranslate', w, -xc, -yc, 0);

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
        KbStrokeWait;

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
