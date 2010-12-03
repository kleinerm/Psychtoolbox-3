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

% History:
% 10/30/05  mk  Wrote it.

theanswer = [];

if nargin < 1
    moviename = [];
    if IsOSX | IsLinux
        theanswer = input('Serious or cool? Type s or c [s/c]? ', 's');
    end        
end;

if isempty(moviename)
    moviename = '*.mov';
end

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
    [win, scr_rect] = Screen('OpenWindow', screen, 0);

    % Retrieve duration of a single video refresh interval:
    ifi = Screen('GetFlipInterval', win);
    
    % Clear screen to background color:
    Screen('FillRect', win, background);
    
    % Initial display and sync to timestamp:
    vbl=Screen('Flip',win);
    iteration=0;    
    abortit=0;

    if isempty(strfind(moviename, 'http'))
        % Return full list of movie files from directory+pattern:
        moviefiles=dir(moviename);
        
        if isempty(moviefiles)
            moviefiles(1).name = [ PsychtoolboxRoot 'PsychDemos/QuicktimeDemos/DualDiscs.mov' ];
        else
            for i=1:size(moviefiles,1)
                moviefiles(i).name = [ pwd filesep moviefiles(i).name ];
            end
        end
        
        moviecount = size(moviefiles,1);
    else
        moviefiles(1).name = moviename;
        moviecount = 1;
    end

    if strcmpi(theanswer, 'c')
        % Cool stuff,downloaded from the web ;-)
        moviefiles(1).name = 'http://movies.apple.com/movies/us/apple/getamac/apple_getamac_group_20080512_480x272.mov';
        moviefiles(2).name = 'http://movies.apple.com/movies/us/apple/getamac/apple_getamac_sadsong_extended_20080519_480x272.mov';
        moviefiles(3).name = 'http://movies.apple.com/movies/us/apple/getamac/apple_getamac_breakthrough_20080401_480x272.mov';
        moviefiles(4).name = 'http://movies.apple.com/movies/us/apple/getamac/apple-getamac-fat_480x376.mov';
        moviefiles(5).name = 'http://movies.apple.com/movies/us/apple/getamac_ads4/prlady_480x272.mov';
        moviecount = size(moviefiles,2);
    end

    % Playbackrate defaults to 1:
    rate=1;
    
    % Endless loop, runs until ESC key pressed:
    while (abortit<2)
        iteration=iteration + 1;
        fprintf('ITER=%i::', iteration);
        moviename=moviefiles(mod(iteration, moviecount)+1).name;
        
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
	    % Only perform video image fetch/drawing if playback is active
	    % and the movie actually has a video track (imgw and imgh > 0):
            if ((abs(rate)>0) & (imgw>0) & (imgh>0))
                % Return next frame in movie, in sync with current playback
                % time and sound.
                % tex either the texture handle or zero if no new frame is
                % ready yet. pts = Presentation timestamp in seconds.
                [tex pts] = Screen('GetMovieImage', win, movie, 1, [], [], 0);

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
