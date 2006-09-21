function PlayMoviesDemoOSX(moviename)
%
% PlayMoviesDemoOSX(moviename)
%
% This demo accepts a pattern for a valid moviename, e.g.,
% moviename='*.mpg', then it plays all movies in the current working
% directory whose names match the provided pattern, e.g., the '*.mpg'
% pattern would play all MPEG files in the current directory.
%
% This demo uses automatic asynchronous playback for synchronized playback
% of video and sound. Each movie plays until end, then rewinds and plays
% again from the start. Pressing the Cursor-Up/Down key pauses/unpauses the
% movie and increases/decreases playback rate.
% The left- right arrow keys jump in 1 seconds steps. SPACE jumps to the
% next movie in the list. ESC ends the demo.
%
% This demo needs MacOS-X 10.3.9 or 10.4.x with Quicktime-7 installed!

% History:
% 10/30/05  mk  Wrote it.

if nargin < 1
    moviename = '*.mov'
end;

% Switch KbName into unified mode: It will use the names of the OS-X
% platform on all platforms in order to make this script portable:
KbName('UnifyKeyNames');

space=KbName('SPACE');
esc=KbName('ESCAPE');
right=KbName('RightArrow');
left=KbName('LeftArrow');
up=KbName('UpArrow');
down=KbName('DownArrow');
shift=KbName('RightShift');

try
    % Child protection
    AssertOpenGL;
    background=[128, 128, 128];

    % Open onscreen window:
    screen=max(Screen('Screens'));
    [win, scr_rect] = Screen('OpenWindow', screen);

    % Retrieve duration of a single video refresh interval:
    ifi = Screen('GetFlipInterval', win);
    
    % Clear screen to background color:
    Screen('FillRect', win, background);
    
    % Initial display and sync to timestamp:
    vbl=Screen('Flip',win);
    iteration=0;    
    abortit=0
    
    % Return full list of movie files from directory+pattern:
    moviefiles=dir(moviename);
    
    % Playbackrate defaults to 1:
    rate=1;
    
    % Endless loop, runs until ESC key pressed:
    while (abortit<2)
        iteration=iteration + 1;
        fprintf('ITER=%i::', iteration);
        moviename=moviefiles(mod(iteration, size(moviefiles,1))+1).name;
        
        % Open movie file and retrieve basic info about movie:
        [movie movieduration fps imgw imgh] = Screen('OpenMovie', win, moviename);
        fprintf('Movie: %s  : %f seconds duration, %f fps, w x h = %i x %i...\n', moviename, movieduration, fps, imgw, imgh);
        
        i=0;
    
        % Seek to start of movie (timeindex 0):
        Screen('SetMovieTimeIndex', movie, 0);
        
        % Start playback of movie. This will start
        % the realtime playback clock and playback of audio tracks, if any.
        % Play 'movie', at a playbackrate = 1, with endless loop=1 and
        % 1.0 == 100% audio volume.
        Screen('PlayMovie', movie, rate, 1, 1.0);
    
        t1 = GetSecs;
        
        % Infinite playback loop: Fetch video frames and display them...
        while(1)
            i=i+1;
            if (abs(rate)>0)
                % Return next frame in movie, in sync with current playback
                % time and sound.
                % tex either the texture handle or zero if no new frame is
                % ready yet. pts = Presentation timestamp in seconds.
                [tex pts] = Screen('GetMovieImage', win, movie, 1);

                % Valid texture returned?
                if tex<=0
                    break;
                end;

                % Draw the new texture immediately to screen:
                Screen('DrawTexture', win, tex);

                % Update display:
                vbl=Screen('Flip', win);

                % Release texture:
                Screen('Close', tex);
            end;
            
            % Check for abortion:
            abortit=0;
            [keyIsDown,secs,keyCode]=KbCheck;
            if (keyIsDown==1 & keyCode(esc))
                % Set the abort-demo flag.
                abortit=2;
                break;
            end;
            
            if (keyIsDown==1 & keyCode(space))
                % Exit while-loop: This will load the next movie...
                break;
            end;
            
            if (keyIsDown==1 & keyCode(right))
                % Advance movietime by one second:
                Screen('SetMovieTimeIndex', movie, Screen('GetMovieTimeIndex', movie) + 1);
            end;

            if (keyIsDown==1 & keyCode(left))
                % Rewind movietime by one second:
                Screen('SetMovieTimeIndex', movie, Screen('GetMovieTimeIndex', movie) - 1);
            end;

            if (keyIsDown==1 & keyCode(up))
                % Increase playback rate by 1 unit.
                if (keyCode(shift))
                    rate=rate+0.1;
                else
                    while KbCheck; WaitSecs(0.01); end;
                    rate=round(rate+1);
                end;
                Screen('PlayMovie', movie, rate, 1, 1.0);
            end;

            if (keyIsDown==1 & keyCode(down))
                % Decrease playback rate by 1 unit.
                if (keyCode(shift))
                    rate=rate-0.1;
                else
                    while KbCheck; WaitSecs(0.01); end;
                    rate=round(rate-1);
                end;
                Screen('PlayMovie', movie, rate, 1, 1.0);
            end;
        end;
    
        telapsed = GetSecs - t1
        finalcount=i

        Screen('Flip', win);
        while KbCheck; end;
        
        % Done. Stop playback:
        Screen('PlayMovie', movie, 0);

        % Close movie object:
        Screen('CloseMovie', movie);
    end;
    
    % Close screens.
    Screen('CloseAll');

    % Done.
    return;
catch
    % Error handling: Close all windows and movies, release all ressources.
    Screen('CloseAll');
end;
