function LoadMovieIntoTexturesDemoOSX(moviename, fromTime, toTime, benchmark)
%
% LoadMovieIntoTexturesDemoOSX(moviename, fromTime, toTime, benchmark)
%
% A demo implementation on how to load a Quicktime movie into standard
% Psychtoolbox textures for precisely controlled presentation timing and
% presentation order.
%
% Parameters:
% moviename - Filename of moviefile to use. If none is provided, then the
% simple DualDiscs collision movie is used which is part of PTB.
%
% fromTime - Start time (in seconds) from which the movie should be read
% into textures. Defaults to start of movie, if not provided.
%
% toTime - End time (in seconds) upto which the movie should be read
% into textures. Defaults to end of movie, if not provided.
%
% benchmark - If you set this parameter to 1, the demo will compute the
% time it takes to load the movie and the maximum display speed without
% syncing to vertical refresh. All visual progress feedback is disabled.
% This is mostly useful to benchmark different movie file formats and
% codecs for their relative efficiency on a given machine.
%
% How the demo works: Read the source code - its well documented ;-)
%
% This demo "preloads" the movie into textures:
% The whole movie gets read into PTB textures before start of trial. Then
% you show the textures in quick succession like in MovieDemoOSX. The
% advantage of this approach is exact control over display timing and
% display order: You can show frames in any order you like at any rate you
% like (and that your hardware likes). Disadvantage: Longer trial
% setup time for loading the whole movie, higher memory consumption (keep
% all n frames in memory instead of only the current one) and the inability
% to play sound in sync with video.
%
% To make it a bit more interesting, you can use this demo to "browse" short
% videoclips, e.g., for selection of interesting parts...
%
%
% This demo needs MacOS-X 10.3.9 or 10.4.x with Quicktime-7 installed!

% History:
% 12/25/05  mk  Wrote it.
% 02/03/06  mk  Adapted for use on Windows.

% Child protection: Make sure we run on the OSX / OpenGL Psychtoolbox.
% Abort if we don't:
AssertOpenGL;

% Switch KbName into unified mode: It will use the names of the OS-X
% platform on all platforms in order to make this script portable:
KbName('UnifyKeyNames');

esc=KbName('ESCAPE');
space=KbName('SPACE');
right=KbName('RightArrow');
left=KbName('LeftArrow');

if nargin < 1
    % Default movie is our own disc collision movie:
    moviename = [ PsychtoolboxRoot 'PsychDemos/QuicktimeDemos/DualDiscs.mov' ];
end;

if nargin < 2
    % Default to beginning of movie:
    fromTime=0;
end;

if nargin < 3
    % Default to end of movie:
    toTime=100000;
end;

if nargin < 4
    benchmark=0;
end;

fprintf('Loading movie %s ...\n', moviename);
try    
    % Background color will be a grey one:
    background=[128, 128, 128];

    % Open onscreen window. We use the display with the highest number on
    % multi-display setups:
    screen=max(Screen('Screens'));
    % This will open a screen with default settings, aka black background,
    % fullscreen, double buffered with 32 bits color depth:
    win = Screen('OpenWindow', screen); % , 0, [0 0 800 600]);
    
    % Hide the mouse cursor:
    HideCursor;
    
    % Clear screen to background color:
    Screen('FillRect', win, background);
    
    % Show instructions...
    tsize=20;
    Screen('TextSize', win, tsize);
    [x, y]=Screen('DrawText', win, 'Loading movie into textures. Please wait...',40, 100);    
    % Flip to show the startup screen:
    Screen('Flip',win);
        
    % Open the moviefile and query some infos like duration, framerate,
    % width and height of video frames...
    [movie movieduration fps imgw imgh] = Screen('OpenMovie', win, moviename);

    % Move to requested timeindex where texture loading should start:
    Screen('SetMovieTimeIndex', movie, fromTime);
    
    movietexture=0;     % Texture handle for the current movie frame.
    lastpts=-1;          % Presentation timestamp of last frame.
    pts=-1;
    count=0;            % Number of loaded movie frames.
    
    tloadstart=GetSecs;
    
    % Movie to texture conversion loop:
    while(movietexture>=0 & pts < toTime)
            % This call waits for arrival of a new frame from the movie. If
            % a new frame is ready, it converts the video frame into a
            % Psychtoolbox texture image and returns a handle in
            % 'movietexture'. 'pts' contains a so called presentation
            % timestamp. That is the time (in seconds since start of movie)
            % at which this video frame should be shown on the screen.
            % In case that no valid video texture is available, this function
            % will return a zero texture handle to indicate this. If no new
            % texture will become available anymore, because the end of the
            % movie is reached, it will return a handle of -1 to indicate end
            % of movie.
            % The 1 - flag means: Wait for arrival of new frame. Next
            % invocation of this command will retrieve the next frame in
            % the movie.
            [movietexture pts] = Screen('GetMovieImage', win, movie, 1);
            
            % Valid and *new* texture? If pts would be *smaller* than
            % lastpts then we would have ran over the end of movie - in
            % that case, the time will automatically wrap around to zero.
            % If we don't check for this, we'll have an infinite loop!
            if (movietexture>0 & pts>lastpts)
                % Store its texture handle and exact movie timestamp in
                % arrays for later use:
                count=count + 1;
                texids(count)=movietexture;
                texpts(count)=pts;
                lastpts=pts;
            else
                break;
            end;

            % Allow for abortion...
            if KbCheck
                break;
            end;
            
            if (benchmark==0)
                % Show the progress text:
                [x, y]=Screen('DrawText', win, ['Loaded texture ' num2str(count) '...'],40, 100);
                Screen('Flip',win);
            end;
    end;
    
    if (benchmark>0)
        % Compute movie load & conversion rate in frames per second.
        loadrate = count / (GetSecs - tloadstart);
        % Compute same rate in Megapixels per second:
        loadvolume = loadrate * imgw * imgh / 1024 / 1024;

        fprintf('Movie to texture conversion speed is %f frames per second == %f Megapixels/second.\n', loadrate, loadvolume);
    end;

    % Ok, now the requested part of the movie has been (hopefully) loaded
    % and converted into standard PTB textures. We can simply use the
    % 'DrawTexture' command in a loop to show the textures...
    
    % Clear screen to background color:
    Screen('Flip',win);
    
    currentindex=1;
    autoplay=0;
    
    totalcount = 0;
    tstart=GetSecs;
    
    % Browse and Draw loop:
    while(count>0)
        % Draw texture 'currentindex'
        Screen('DrawTexture', win, texids(currentindex));

        if (benchmark==0)
            % Draw some help text:
            [x, y]=Screen('DrawText', win, 'Press left-/right cursor key to navigate in movie, SPACE to toggle playback, ESC to exit.',10, 40);
            [x, y]=Screen('DrawText', win, ['Framerate(fps): ' num2str(fps) ', total duration of movie (secs)' num2str(movieduration)],10, y+10+tsize);

            % Draw info on current position in movie:
            [x, y]=Screen('DrawText', win, ['Frame ' num2str(currentindex) ' of ' num2str(count) ' : Timeindex(secs) = ' num2str(texpts(currentindex))], 10, y + 10 + tsize);

            % Show drawn stuff:
            Screen('Flip', win);

            % Check for key press:
            [keyIsDown, secs, keyCode]=KbCheck;
            if keyIsDown
                if (keyCode(esc))
                    % Exit
                    break;
                end;

                if (keyCode(space))
                    % Toggle playback on space.
                    autoplay=1-autoplay;
                end;

                if (keyCode(right) & currentindex<count)
                    % One frame forward:
                    currentindex=currentindex+1;
                end;
                if (keyCode(left) & currentindex>1)
                    % One frame backward:
                    currentindex=currentindex-1;
                end;

                % Wait for key-release:
                while KbCheck; WaitSecs(0.01); end;
            end;
        else
            % Benchmark mode: Repeat until Keypress.
            Screen('Flip', win, 0, 2, 2);
            totalcount=totalcount + 1;
            autoplay=1;
            
            if KbCheck
                break;
            end;
        end;
        
        % Update frameindex if in autoplay mode:
        if autoplay==1
            currentindex=mod(currentindex, count) + 1;
        end;
    end;
    
    % Done. Flip a last time to show grey background:
    Screen('Flip', win);
    
    if (benchmark>0)
        playbackrate = totalcount / (GetSecs - tstart);
        playbackvolume = playbackrate * imgw * imgh / 1024 / 1024;
        fprintf('Movietexture playback rate is %f frames per second == %f Megapixels/second.\n', playbackrate, playbackvolume);
    end;
    
    % Close movie file. This will also release all textures...
    Screen('CloseMovie', movie);
   
    ShowCursor;
    Screen('CloseAll');
    fprintf('Done. Bye!\n');
    return;

catch
    % Error handling: Close all windows and movies, release all ressources.
    ShowCursor;
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end;
