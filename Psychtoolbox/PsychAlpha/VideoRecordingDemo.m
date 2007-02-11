function VideoRecordingDemo(moviename, withsound, showit)
% VideoRecordingDemo(moviename [, withsound] [, showit])
%
% Demonstrates simple video capture and recording to a Quicktime movie
% file. Only works on OS/X and Windows for now. Only works with default
% codec and encoding parameters for now.
%
% 'moviename' name of output movie file. The file must not exist at start
% of recording. 'withsound' If set to non-zero, sound will be recorded as
% well (default: record sound). 'showit' If non-zero, video will be shown
% onscreen during recording (default: Show it).
%
% This is early beta code, expect bugs and rough edges!

% History:
% 11.2.2007 Written (MK).

AssertOpenGL;

if ~IsOSX & ~IsWin
    error('Sorry, this demo currently only works on OS/X and Windows.');
end

screen=max(Screen('Screens'));

if nargin < 1
    error('You must provide a quicktime output movie name as first argument!');
end
moviename

if exist(moviename)
    error('Moviefile must not exist! Delete it or choose a different name.');
end

if nargin < 2 || isempty(withsound)
    withsound = 2;
end

if withsound > 0
    withsound = 2;
end

if nargin < 3
    showit = 1;
end

if showit > 0
    waitforimage = 1;
else
    waitforimage = 2;
end

try
    InitializeMatlabOpenGL;
    
    win=Screen('OpenWindow', screen, 0);

    % Initial flip to a blank screen:
    Screen('Flip',win);

    % Set text size for info text. 24 pixels is also good for Linux.
    Screen('TextSize', win, 24);
            
    % Capture video + audio to disk:
    grabber = Screen('OpenVideoCapture', win, 0, [0 0 640 480],[] ,[], [] , moviename, withsound);
    brightness = Screen('SetVideoCaptureParameter', grabber, 'Brightness',383)

    % Start capture, request 60 fps. Capture hardware will fall back to
    % fastest supported if its not supported (i think).
    Screen('StartVideoCapture', grabber, 60, 1);

    tex = 0;
    oldpts = 0;
    count = 0;
    t=GetSecs;
    while (GetSecs - t) < 600 
        % Check for keypress, abort recording, if any:
        if KbCheck
            break;
        end;
        
        % Wait blocking for next image. If waitforimage == 1 then return it
        % as texture, if waitforimage == 2, do not return it (no preview,
        % but faster).
        [tex pts nrdropped]=Screen('GetCapturedImage', win, grabber, waitforimage);
        % fprintf('tex = %i  pts = %f nrdropped = %i\n', tex, pts, nrdropped);
        
        % If a texture is available, draw, show and release it.
        if (tex>0)
            % Draw new texture from framegrabber.
            Screen('DrawTexture', win, tex, [], [], 0, 0); %Screen('Rect', win));

            % Print pts:
            %            Screen('DrawText', win, sprintf('%.4f', pts - t), 0, 0, 255);
            if count>0
                % Compute delta:
                delta = (pts - oldpts) * 1000;
                oldpts = pts;
                %               Screen('DrawText', win, sprintf('%.4f', delta), 0, 20, 255);
            end;

            % Show it.
            Screen('Flip', win);
            Screen('Close', tex);
            tex=0;
        end;        
        count = count + 1;
    end;
    % Done. Shut us down.
    telapsed = GetSecs - t
    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);
    Screen('CloseAll');
    avgfps = count / telapsed
catch
   Screen('CloseAll');
end;
