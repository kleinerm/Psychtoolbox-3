function VideoFeedbackLoop(fps, fifodelay, mirrored, fullscreen)
% VideoFeedbackLoop(fps, fifodelay, mirrored, fullscreen)
%
% Demo implementation of a simple video feedback loop with controllable
% latency.
%
% This demo implements a simple video feedback loop.
%

% History:
% 28.04.06  Written (MK)

% Check if we're running on OpenGL-Psychtoolbox:
AssertOpenGL;

if IsLinux
   esckey=KbName('Escape');
end;

if IsWin
   esckey=KbName('ESC');
end;

% Assign defaults for our function arguments:
if nargin < 1 | isempty(fps)
    fps = 60; % Default to 60 frames per second capture rate.
end;
fps

if nargin < 2 | isempty(fifodelay)
    fifodelay = 0; % Default to zero delay. Show as fast as possible.
end;
fifodelay=round(fifodelay)

if nargin < 3 | isempty(mirrored)
    mirrored = 0; % Default to non-mirrored video view.
end;
mirrored

if nargin < 4 | isempty(fullscreen)
    fullscreen=1; % Default to fullscreen view.
end;
fullscreen

% Find id of highest physical display screen in system for display.
% This does nothing on single-display systems. On dual-displays it
% will show the video feedback on the secondary display instead of
% the one with the Matlab window and Desktop...
screen=max(Screen('Screens'));

%try
    % Open "normal" window or fullscreen window, double-buffered,
    % with black background color.
    if fullscreen<1
        win=Screen('OpenWindow', screen, 0, [0 0 800 600]);
    else
        win=Screen('OpenWindow', screen, 0);
    end;
    
    % Show black initial screen after init:
    Screen('Flip',win);
    
    % Open default video capture device with default settings, except
    % for ROI which is 640 x 480 pixels:
    grabber = Screen('OpenVideoCapture', win, [], [0 0 640 480]);

    % Query camera capture settings if supported:
    brightness = Screen('SetVideoCaptureParameter', grabber, 'Brightness')
    exposure = Screen('SetVideoCaptureParameter', grabber, 'Exposure')
    gain = Screen('SetVideoCaptureParameter', grabber, 'Gain', 120)
    shutter = Screen('SetVideoCaptureParameter', grabber, 'Shutter', 500)

    % Print all camera capabilities:
    Screen('SetVideoCaptureParameter', grabber, 'PrintParameters')
    
    % Query real monitor refresh interval (in seconds) as computed by
    % Psychtoolbox internal calibration:
    ifi = Screen('GetFlipInterval', win);

    % Translate latency in frames into latency in milliseconds:
    latencymillisecs = fifodelay * 1/fps * 1000.0;
    fprintf('Requested latency in milliseconds: %f\n', latencymillisecs);

    displayfps = 1.0 / ifi

    % Allocate timestamp buffers for 600 seconds:
    capturetimes= zeros(1, 600 * fps);
    delay=zeros(1, 600 * fps);

    % Allocate ringbuffer of texture handles and capture timestamps
    % for implementation of the delay loop:
    videofifo = zeros(2, fifodelay+1);

    % Preinit capture counter: It runs 'fifodelay' frames ahead of read counter:
    capturecount = fifodelay;

    % Setup our recycled texture handle: Initially it is zero which means
    % "No old unneeded texture available for recycling"...
    recycledtex = 0;

    % Start video capture with fps frames per second, if possible. Use
    % low-latency capture by dropping frames, if necessary:
    capturefps = Screen('StartVideoCapture', grabber, fps, 1)

    if abs(displayfps - capturefps)> 0.5
       fprintf('WARNING: Non-matching capture framerate and display framerate - expect temporal Aliasing artifacts!!!\n');
    end;

    % Record start time of feedback:
    tstart=Screen('Flip', win);
    oldcts = tstart;

    % Video capture and feedback loop. Runs until keypress or 600 secs have passed:
    while (GetSecs - tstart) < 600 
        if KbCheck
            [down secs keycode]=KbCheck;
            if keycode(esckey)
                break;
            end;
        end;
        
        % Retrieve most recently captured image from video source, block if none is
        % available yet. If recycledtex is a valid handle to an old, no longer needed
        % texture, the capture engine will recycle it for higher efficiency:
        [tex cts nrdropped]=Screen('GetCapturedImage', win, grabber, 1, recycledtex);
        % recycledtex has been used up. Null it out:
        recycledtex = 0;

        % fprintf('tex = %i  cts = %f tcur = %f\n', tex, cts, GetSecs);

        % Frame dropped during this capture cycle?
        if nrdropped > 0
            fprintf('Frame dropped: %i, %f\n', capturecount, (cts-oldcts)*1000);
        end;

        if (cts < oldcts)
            fprintf('BUG! TIMESTAMP REVERSION AT FRAME %i, DELTA = %f !!!\n', capturecount, (cts-oldcts)*1000);
        end;

        oldcts=cts;

        % New image captured and returned as texture?        
        if (tex>0)
	   % Yes. Put it into our fifo ringbuffer, together with the requested presentation
	   % deadline for that image, which is the capture timestamp + requested delay:
           capturecount = capturecount + 1;
           writeptr = mod(capturecount, size(videofifo, 2)) + 1;
           videofifo(1, writeptr) = tex;
           videofifo(2, writeptr) = cts + (latencymillisecs / 1000.0);
           capturetimes(capturecount)=cts;
           % Done with capture of this frame...

           % Now read out the frame some fifo delay slots behind and show it:
           readcount = capturecount - fifodelay;
           readptr = mod(readcount, size(videofifo, 2)) + 1;

           % Get pts and texture handle for image to show:
           pts = videofifo(2, readptr);
           tex = videofifo(1, readptr);

           % Nothing to show yet?
           if pts == 0 | tex == 0
              % Skip remaining loop:
	      continue;
           end;

           % We have a texture to show. Do it:
           Screen('DrawTexture', win, tex);
           videofifo(1, readptr)=0;

           tonset = Screen('Flip', win, 0, 2);
           delay(readcount) = tonset - capturetimes(readcount);

           % We don't need texture 'tex' anymore. Put it into 'recycledtex',
           % so the framecapture engine can reuse it for faster processing.
           recycledtex = tex;

           % Release the image to free up memory:
           % Not used anymore: We recycle textures instead! Screen('Close', tex);
        end;

        % We've processed the fifo content. Repeat the loop to see
        % if new frames are ready for display.
    end;

    % Done with video feedback loop. Shutdown video capture:
    telapsed = GetSecs - tstart

    % Stop capture, do the stats:
    droppedincapturedevice = Screen('StopVideoCapture', grabber);
    droppedincapturedevice

    % Shutdown video source:
    Screen('CloseVideoCapture', grabber);

    % Close window and release all other ressources:
    Screen('CloseAll');

    plot(delay(1:readcount)*1000);

    avgfps = readcount / telapsed

    % Well done!
    return;

%catch
    % Shutdown video source:
    Screen('CloseVideoCapture', grabber);
    Screen('CloseAll');
%end;
