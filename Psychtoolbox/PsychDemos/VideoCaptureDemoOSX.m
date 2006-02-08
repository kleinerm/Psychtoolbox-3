function VideoCaptureDemoOSX

AssertOSX;
screen=max(Screen('Screens'));

try
    win=Screen('OpenWindow', screen);
    Screen('FillRect', win, 0);
    Screen('Flip',win);

    [grabber fps width height]=Screen('OpenVideoCapture', win, 0);
    fprintf('Grabber running at %i Hz width x height = %i x %i', fps, width, height);
    fps
    width
    Screen('StartVideoCapture', grabber);

    oldpts = 0;
    count = 0;
    t=GetSecs;
    while (GetSecs - t) < 60 
        if KbCheck
            break;
        end;

        [tex pts ]=Screen('GetCapturedImage', win, grabber, 0);
        % fprintf('tex = %i ::', tex);
        
        if (tex>0)
            % Print pts:
            Screen('DrawText', win, num2str(pts), 0, 0, 255);
            if count>0
                % Compute delta:
                delta = (pts - oldpts) * 1000;
                oldpts = pts;
                Screen('DrawText', win, num2str(delta), 0, 20, 255);
            end;
            
            % New texture from framegrabber.
            Screen('DrawTexture', win, tex);
            Screen('Flip', win, 0, 0, 2);
            Screen('Close', tex);
            tex=0;
        end;
        
        % WaitSecs(0.1);
        
        count = count + 1;
    end;
    telapsed = GetSecs - t
    
    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);

    Screen('CloseAll');

catch
    Screen('CloseAll');
end;
