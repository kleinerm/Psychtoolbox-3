function VideoCaptureLatencyTest(fullscreen)
% VideoCaptureLatencyTest
%
% This routine tests the latency of Videocapture for
% a given Camera -> Computer -> PTB -> gfx-card -> display combo.
%

% Running OpenGL-PTB? This is required.
AssertOpenGL;

% Find stimulus display:
screen=max(Screen('Screens'));
if nargin < 1
   fullscreen=0;
end;

fullscreen

try
    % Open display with default settings:
    if fullscreen<1
	    win=Screen('OpenWindow', screen, 0, [0 0 800 600]);
    else
       win=Screen('OpenWindow', screen);
    end;
    
    % Setup font and textsize:
    Screen('TextSize', win, 30);
    Screen('TextStyle', win, 1);
    
    % Clear out and show black display:
    Screen('FillRect', win, 0);
    Screen('Flip',win);
    
    % Open default video capture device:
    [grabber fps width height]=Screen('OpenVideoCapture', win, 0);
    fprintf('Grabber running at %i Hz width x height = %i x %i\n', fps, width, height);
    
    % Start video capture:
    Screen('StartVideoCapture', grabber);

    oldpts = 0;
    count = 0;
    t=GetSecs;
    while (GetSecs - t) < 600 
        if KbCheck
            break;
        end;
        
        [tex pts intensity]=Screen('GetCapturedImage', win, grabber, 0);
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
            
            % Compute and print intensity:
            intensity = intensity / 640 / 480 / 4;
            Screen('DrawText', win, num2str(intensity), 0, 40, 255);
           
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
