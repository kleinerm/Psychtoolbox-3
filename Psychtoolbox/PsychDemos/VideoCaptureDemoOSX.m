function VideoCaptureDemoOSX(fullscreen)

AssertOpenGL;
screen=max(Screen('Screens'));
if nargin < 1
    fullscreen=0;
end;

try
    if fullscreen<1
        win=Screen('OpenWindow', screen, 0, [0 0 800 600]);
    else
        win=Screen('OpenWindow', screen);
    end;
    
    Screen('FillRect', win, 0);
    Screen('Flip',win);
    
    grabber = Screen('OpenVideoCapture', win, 0, [0 0 640 480]);
    
    Screen('StartVideoCapture', grabber, 60, 1);

    oldpts = 0;
    count = 0;
    t=GetSecs;
    while (GetSecs - t) < 600 
        if KbCheck
            break;
        end;
        
        [tex pts]=Screen('GetCapturedImage', win, grabber, 1);
        % fprintf('tex = %i  pts = %f\n', tex, pts);
        
        if (tex>0)
            % Setup mirror transformation for horizontal flipping:
            
            % xc, yc is the geometric center of the text.
            [xc, yc] = RectCenter(Screen('Rect', win));
            
            % Make a backup copy of the current transformation matrix for later
            % use/restoration of default state:
            Screen('glPushMatrix', win);
            % Translate origin into the geometric center of text:
            Screen('glTranslate', win, xc, 0, 0);
            % Apple a scaling transform which flips the diretion of x-Axis,
            % thereby mirroring the drawn text horizontally:
            Screen('glScale', win, -1, 1, 1);
            % We need to undo the translations...
            Screen('glTranslate', win, -xc, 0, 0);
            % The transformation is ready for mirrored drawing:

            % Draw new texture from framegrabber.
            Screen('DrawTexture', win, tex, [], Screen('Rect', win));

            Screen('glPopMatrix', win);

            % Print pts:
            Screen('DrawText', win, num2str(pts - t), 0, 0, 255);
            if count>0
                % Compute delta:
                delta = (pts - oldpts) * 1000;
                oldpts = pts;
                Screen('DrawText', win, num2str(delta), 0, 20, 255);
            end;
            
            % Show it.
            Screen('Flip', win);
            Screen('Close', tex);
            tex=0;
        end;        
        count = count + 1;
    end;
    telapsed = GetSecs - t
    %WaitSecs(1);
    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);
    Screen('CloseAll');

catch
   Screen('CloseAll');
end;
