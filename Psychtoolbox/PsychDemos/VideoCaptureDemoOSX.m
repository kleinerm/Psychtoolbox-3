function VideoCaptureDemoOSX

AssertOpenGL;
screen=max(Screen('Screens'));

try
    win=Screen('OpenWindow', screen, 0, [0 0 800 600]);
    %win=Screen('OpenWindow', screen);
    Screen('TextSize', win, 30);
    Screen('TextStyle', win, 1);
    Screen('FillRect', win, 0);
    Screen('Flip',win);
    
    [grabber fps width height]=Screen('OpenVideoCapture', win, 0);
    fprintf('Grabber running at %i Hz width x height = %i x %i\n', fps, width, height);
    
    Screen('StartVideoCapture', grabber);

    oldpts = 0;
    count = 0;
    t=GetSecs;
    while (GetSecs - t) < 600 
        if KbCheck
            break;
        end;
        
        [tex pts ]=Screen('GetCapturedImage', win, grabber, 0);
        % fprintf('tex = %i  pts = %f\n', tex, pts);
        
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
        count = count + 1;
    end;
    telapsed = GetSecs - t
    
    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);
    Screen('CloseAll');

catch
   Screen('CloseAll');
end;
