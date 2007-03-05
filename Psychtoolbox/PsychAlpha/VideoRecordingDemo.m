function VideoRecordingDemo(moviename, codec, withsound, showit)
% VideoRecordingDemo(moviename [, codec] [, withsound] [, showit])
%
% Demonstrates simple video capture and recording to a Quicktime movie
% file. Only works on OS/X and Windows for now.
%
% Arguments and their meaning:
%
% 'moviename' name of output movie file. The file must not exist at start
% of recording.
%
% 'codec' a number, indicating the type of video codec you want to use.
% Defaults to "whatever the system default is", valid settings are between
% 0 and 9. Some codecs are very fast, i.e., high framerates and low system
% load, others provide high compression rates, i.e., small video files at
% good quality. Usually there's a tradeoff between encoding speed, quality
% and compression ratio, so you'll have to try out different ones to find
% one suitable for your purpose. Some codecs only work at specific
% framerates or for specific image sizes. If you set the level of verbosity
% of PTB to at least 4, it will print out a list of supported codecs on
% your system (Screen('Preference', 'Verbosity', 4)). Without
% Quicktime-Pro, you'll usually only have a very limited set of codecs
% available.
%
% Codecs supported by this demo: 0 = Default, 1 = H.264, 2 = Apple Pixlet,
% 3 = MPEG-4, 4 = Component video, 5 = DV-PAL, 6 = DVCPRO-NTSC, 7 =
% DVCPRO-PAL, 8 = DVCPRO50-NTSC, 9 = DVCPRO50-PAL.
%
% 'withsound' If set to non-zero, sound will be recorded as
% well (default: record sound).
%
%'showit' If non-zero, video will be shown onscreen during recording
% (default: Show it). Not showing the video during recording will
% significantly reduce system load, so this may help to sustain a skip free
% recording on lower end machines.
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

if nargin < 2
   codec = [];
end

if isempty(codec)
    codec = '';
else
    switch(codec)
        case 0,
            codec = '';
        case 1,
            codec = ':CodecType=1635148593'; % H.264 codec.
        case 2,
            codec = ':CodecType=1886940276'; % Apple Pixlet Video codec.
        case 3,
            codec = ':CodecType=1836070006'; % MPEG-4 Video codec.
        case 4,
            codec = ':CodecType=2037741106'; % Component Video codec.  
        case 5,
            codec = ':CodecType=1685480304'; % DV - PAL codec.
        case 6,
            codec = ':CodecType=1685480224'; % DVCPRO - NTSC codec.
        case 7,
            codec = ':CodecType=1685483632'; % DVCPRO - PAL codec.
        case 8,
            codec = ':CodecType=1685468526'; % DVCPRO50 - NTSC codec.
        case 9,
            codec = ':CodecType=1685468528'; % DVCPRO50 - PAL codec.
        otherwise
            error('Unknown codec specified, only know types 0 to 9.');
    end
end

if nargin < 3
   withsound = [];
end

if isempty(withsound)
    withsound = 2;
end

if withsound > 0
    withsound = 2;
end

if nargin < 4
    showit = 1;
end

if showit > 0
    waitforimage = 1;
else
    waitforimage = 2;
end

try
    % Open fullscreen window on 'screen', with black background color:
    win=Screen('OpenWindow', screen, 0);

    % Initial flip to a blank screen:
    Screen('Flip',win);

    % Set text size for info text. 24 pixels is also good for Linux.
    Screen('TextSize', win, 24);
            
    % Capture video + audio to disk:
    grabber = Screen('OpenVideoCapture', win, 0, [0 0 640 480],[] ,[], [] , [moviename codec], withsound);

    % Start capture, request 60 fps. Capture hardware will fall back to
    % fastest supported framerate if its not supported (i think).
    Screen('StartVideoCapture', grabber, 60, 1);

    oldtex = 0;
    oldpts = 0;
    count = 0;
    t=GetSecs;
    
    % Run until keypress or until 600 seconds have elapsed.
    while (GetSecs - t) < 600 
        % Check for keypress, abort recording, if any:
        if KbCheck
            break;
        end;
        
        % Wait blocking for next image. If waitforimage == 1 then return it
        % as texture, if waitforimage == 2, do not return it (no preview,
        % but faster). oldtex contains the handle of previously fetched
        % textures - recycling is not only good for the environment, but also for speed ;)
        [tex pts nrdropped]=Screen('GetCapturedImage', win, grabber, waitforimage, oldtex);

        % Some output to the console:
        % fprintf('tex = %i  pts = %f nrdropped = %i\n', tex, pts, nrdropped);
        
        % Print pts:
        Screen('DrawText', win, sprintf('%.4f', pts - t), 0, 0, 255);
        if count>0
            % Compute delta:
            delta = (pts - oldpts) * 1000;
            oldpts = pts;
            Screen('DrawText', win, sprintf('%.4f', delta), 0, 20, 255);
        end;

        % If a texture is available, draw, show and release it.
        if (tex>0)
            % Draw new texture from framegrabber.
            Screen('DrawTexture', win, tex);
            
            % Recycle this texture - faster:
            oldtex = tex;
        end;        

        % Show it.
        Screen('Flip', win);

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
