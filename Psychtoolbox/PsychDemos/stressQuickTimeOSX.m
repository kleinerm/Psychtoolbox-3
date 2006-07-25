function stressQuickTimeOSX(moviename, prefetch, durationSecs)
%
% stressQuickTimeOSX(moviename, prefetch, durationSecs)
%
% Stress test for OS-X movie playback function. Opens moviefile
% 'moviename' repeatedly. Sequence is:
% 1. Open 'moviename'
% 2. Play back moviename for at most 'durationSecs' seconds.
% 3. Close movie.
% 4. Go to step 1.
%
% This test runs until the user presses a key on the keyboard.
% The 'prefetch' parameter defines, if the movie should be prefetched into
% a stack of Psychtoolbox textures and then played back from this stack, or
% if the movie should be played on-the-fly in a streaming fashion, like any
% conventional movie player would do.
%
% prefetch=1 -- Fetch movie into textures, then play back in loop: Full
% control over timing, but no sound output and large amount of memory
% needed.
% prefetch=0 -- On-the-fly playback: Minimum memory consumption, no preload
% delay, synchronized sound output, but higher chance of timing jitter /
% playback stutter.
%
%
% This demo needs MacOS-X 10.3.9 or 10.4.x with Quicktime-7 installed!
%
% History:
% 10/30/05  mk  Wrote it.

try
   AssertOSX;
catch
	error('This demo does not work under M$-Windows yet, only on MacOS-X. Aborting...');   
end


if nargin < 2
    prefetch=0
end;

if nargin < 1
    moviename = '/Users/kleinerm/Desktop/*.mpg'
end;

if nargin < 3
    durationSecs = 1000
end;

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

    if prefetch==1
        %Priority(9);
    end;
    
    % Initial display and sync to timestamp:
    vbl=Screen('Flip',win);
    iteration=0;
    
    abortit=0
    
    % Return full list of movie files from directory+pattern:
    moviefiles=dir(moviename);
    
    % Endless loop:
    while (abortit<2)
        iteration=iteration + 1;
        fprintf('ITER=%i::', iteration);
        moviename=moviefiles(mod(iteration, size(moviefiles,1))+1).name;
        
        % Open movie file and retrieve basic info about movie:
        if prefetch==1
            [movie movieduration fps imgw imgh framecount] = Screen('OpenMovie', win, moviename);
        else
            [movie movieduration fps imgw imgh] = Screen('OpenMovie', win, moviename);
            framecount=0;
        end;
        
        fprintf('Movie: %s  : %i frames, %f seconds duration, %f fps...\n', moviename, framecount, ...
                movieduration, fps);
        
        if prefetch==1
            % Preallocate texids:
            texids=zeros(framecount+1,1);    
        end;
        
        i=0;
    
        % Seek to start of movie (timeindex 0):
        Screen('SetMovieTimeIndex', movie, 0);
        % Start playback of movie if in non-prefetch mode. This will start
        % the realtime playback clock and playback of audio tracks, if any.
        if prefetch==0
            % Play 'movie', at a playbackrate = 1, with endless loop=1 and
            % 1.0 == 100% audio volume.
            Screen('PlayMovie', movie, 1, 1, 1.0);
        end;
    
        t1 = GetSecs;
    
    
        % Infinite playback loop: Fetch video frames and display them...
        % We abort the loop in playback after durationSecs have elapsed, or
        % after all 'framecount' frames have been fetched into textures if
        % we are in prefetch-mode.
        while((prefetch==0 & Screen('GetMovieTimeIndex', movie) <= durationSecs) | ...
              (prefetch==1 & i < framecount))
            i=i+1;

            % Fetch next frame from Movie and convert it to new texture:
            % This shows how polling for new frames is done, useful if you
            % want to do other stuff in parallel, e.g., KbChecks and
            % such...
            tex = 0;
            while (tex==0)
                % Return next frame in movie, in sync with current playback
                % time if playback is active. If in prefetch-mode, this
                % will just return the next frame in the movie. tex is
                % either the texture handle or zero if no new frame is
                % ready yet. pts = Presentation timestamp in seconds.
                [tex pts] = Screen('GetMovieImage', win, movie, 1); % , Screen('GetMovieTimeIndex', movie));
                % We'll sleep 4 ms in order to not overload the machine.
                %WaitSecs(0.004);

                if (prefetch==1)
                     fprintf('Frame %i : tpts= %f  tafter = %f\n', i, pts, Screen('GetMovieTimeIndex', movie));
                     if KbCheck
                        abortit=1;
                        break;
                     end;
                end;
            end;
        
            % Valid texture returned?
            if tex==-1
                break;
            end;
     
            % Prefetch mode or live streaming playback?
            if prefetch==0
                % Draw the new texture immediately to screen:
                Screen('DrawTexture', win, tex, [], [], 0);

                % Update display:
                vbl=Screen('Flip', win);

                % Release texture:
                Screen('Close', tex);
            else
                % Prefetch: We just 'store' the texture in texids for later
                % drawing...
                texids(i)=tex;
            end;
            
            % Check for abortion:
            abortit=0;
            [keyIsDown,secs,keyCode]=KbCheck;
            if (keyIsDown==1 & keyCode(KbName('ESCAPE')))
                abortit=2;
                break;
            end;
            
            if (keyIsDown==1 & keyCode(KbName('SPACE')))
                break;
            end;
            
        end;
    
        telapsed = GetSecs - t1
        finalcount=i

        Screen('Flip', win);
        while KbCheck; end;
        
        if (prefetch==0 | abortit>0)
           % Done. Stop playback:
            Screen('PlayMovie', movie, 0);

            % Close movie object:
            Screen('CloseMovie', movie);
        else
            % Play back the stack of texids textures in a loop:
            j=i;
            t1=Screen('Flip', win);
            vbl=t1;
            for i=1:j
                % Draw the new texture to screen:
                Screen('DrawTexture', win, texids(i), [], [], 0);
            
                % Update display:
                vbl=Screen('Flip', win, vbl + (1.0/fps) - 0.5*ifi);
                                
                if KbCheck
                    abortit=1;
                    break;
                end;
            end;
            telapsedfast=GetSecs-t1

            % Release all textures:
            Screen('Close');
            % Close movie object:
            Screen('CloseMovie', movie);
        end;
    end;
    
    % Close screens.
    Priority(0);
    Screen('CloseAll');

    % Done.
    return;
catch
    % Error handling: Close all windows and movies, release all ressources.
    Priority(0);
    Screen('CloseAll');
end;
