function VideoDVCamCaptureDemo(fullscreen, fullsize, roi, depth, deviceId, moviename)
% Demonstrate simple use of built-in video capture engine with DV consumer cameras.
%
% VideoDVCamCaptureDemo([fullscreen=0][, fullsize=0][, roi][, depth][,deviceId=0][, moviename])
%
% VideoDVCamCaptureDemo initializes the first attached and supported DV firewire
% consumer camera, then shows its video image in a Psychtoolbox window.
% A press of the ESCape key ends the demo.
%
% Optional parameters:
%
% 'fullscreen' If set to non-zero value, the image is displayed in a
% fullscreen window, as usual, otherwise a normal GUI window is used.
%
% 'fullsize' If set to 1, the cameras image is scaled up to full screen
% resolution, ie. so it fills the maximum amount of display area, but
% preserving the original aspect ratio.
%
% 'roi' Set to [0 0 720 576] for a PAL-DV camera and [0 0 720 480] for a NTSC-DV camera.
% Default is PAL if omitted.
%
% 'deviceId' Device index of video capture device. Defaults to system default. You can
% also specify a gst-launch style string to define a videosource here. Or you can set
% the special string deviceId = 'X' so builtin spec strings suitable for each operating
% system will be used.
%
% 'moviename' Name string for selection of filename of a target movie file to
% which video should be recorded. Defaults to none,ie., no video recording.
%

% History:
% 27-Dec-1013  mk  Written.

PsychDefaultSetup(1);

if nargin < 1
    fullscreen=[];
end

if isempty(fullscreen)
    fullscreen=0;
end

if nargin < 2
    fullsize=[];
end

if isempty(fullsize)
    fullsize=0;
end

if nargin < 3 || isempty(roi)
    % Must set explicit roi with video resolution. Only two options,
    % 720 x 576 for PAL DV cameras, and 720 x 480 for NTSC DV cameras.
    % We choose PAL:
    roi = [0 0 720 576];
end

if nargin < 4
    depth = [];
end

if nargin < 5
    deviceId = [];
end

if nargin < 6
    moviename = [];
end

recordingflags = 0;
if ~isempty(moviename)
  % For movie recording, also enable audio recording.
  % Note: Audio is *not* actually recorded from the DV
  % camera, but from the systems soundcard, or whatever
  % was selected via video recording settings.
  % However, enabling use of audio recording seems to sometimes
  % avoid hangs of DV recording at least on Linux on repeated
  % sessions, so this is essentially a hack, even if no audio is
  % needed:
  recordingflags = 2;
 end
 
screenid=max(Screen('Screens'));

try
    if fullscreen<1
        win=Screen('OpenWindow', screenid, 0, [0 0 800 600]);
    else
        win=Screen('OpenWindow', screenid, 0);
    end;
    
    % Initial flip to a blank screen:
    Screen('Flip',win);
    
    % Set text size for info text to 24 pixels.
    Screen('TextSize', win, 24);
    
    % Use of a gst-launch style spec for a video source requested? A deviceId which
    % is actually a character string would signal this:
    if ischar(deviceId)
      % Yes. Meaningful spec string, or only a single dummy character?
      if length(deviceId) < 2
        % Dummy: Replace by hard-coded "known to work" example string:
        
        if IsLinux
          % Any of these work on Linux:
          capturebinspec = 'dv1394src ! dvdemux ! dvdec name=ptbdvsource';
          %capturebinspec = 'dv1394src ! video/x-dv ! dvdemux ! dvdec name=ptbdvsource';
          %capturebinspec = 'dv1394src ! dvdemux ! ffdec_dvvideo name=ptbdvsource';
        end
        
        if IsWin
          % This should work on MS-Windows if Matthew Edmundson is right:
          capturebinspec = 'dshowvideosrc ! video/x-dv ! dvdemux ! dvdec name=ptbdvsource';
        end
        
        if IsOSX
          % This should work on OSX, if at all. If this doesn't work then game-over (Untested):
          capturebinspec = 'qtkitvideosrc name=ptbdvsource';
        end
      else
        % Real string: Use it.
        capturebinspec = deviceId;
      end

      if isempty(roi)
        error('You *must* specify a roi of [0 0 720 576] for PAL-DV or [0 0 720 480] for NTSC-DV if you define your own video capture source via deviceId as a string!');
      end
      
      % Assign capturebinspec as gst-launch style capture bin spec for use as video source:
      Screen('SetVideoCaptureParameter', -1, sprintf('SetNextCaptureBinSpec=%s', capturebinspec));
      
      % Signal to Screen() that spec string should be used. This via special deviceId -9:
      deviceId = -9;
    end
    
    % Open DV camera: We always use engineId 3 for GStreamer, as only GStreamer supports DV cameras atm.:
    grabber = Screen('OpenVideoCapture', win, deviceId, roi, depth, [], [], moviename, recordingflags, 3);

    if IsWin || IsOSX || (IsLinux && ~isempty(getenv('PSYCH_FORCE_CAMERABIN2')))
      % If user-specified video source string is in use, then we refrain from setting
      % fps here, but use the "do what you think is right" 'realmax' joker. This one only
      % works with the GStreamer camerabin2 video capture plugin. We use camerabin2
      % on Apple/OSX and MS-Windows by default, but only optionally on Linux, so only
      % do this if we are on Windows, OSX or Linux with camerabin2 enabled:
      Screen('StartVideoCapture', grabber, realmax, 1);      
    else
      % For DV cameras you *must* specify the exact capture framerate of which the
      % camera is capable of. For PAL DV cams this is 25 fps, for NTSC DV cams this
      % is probably either 30 fps or 30000/1001 fps. Any other framerate setting *will fail* !
      if RectHeight(roi) == 576
        % PAL-DV:
        Screen('StartVideoCapture', grabber, 25, 1);
      else
        % NTSC-DV:
        Screen('StartVideoCapture', grabber, 30000/1001, 1);
      end
    end
    
    dstRect = [];
    oldpts = 0;
    count = 0;
    t=GetSecs;
    
    while (GetSecs - t) < 600
        if KbCheck
            break;
        end;
        
        [tex pts nrdropped,intensity]=Screen('GetCapturedImage', win, grabber, 1); %#ok<ASGLU,NASGU>
        % fprintf('tex = %i  pts = %f nrdropped = %i\n', tex, pts, nrdropped);
        
        if (tex>0)
            % Perform first-time setup of transformations, if needed:
            if fullsize && (count == 0)
                texrect = Screen('Rect', tex);
                winrect = Screen('Rect', win);
                sf = min([RectWidth(winrect) / RectWidth(texrect) , RectHeight(winrect) / RectHeight(texrect)]);
                dstRect = CenterRect(ScaleRect(texrect, sf, sf) , winrect);
            end
            
            % texinfo = Screen('Getwindowinfo', tex)
            % outintens = intensity
            
            % Draw new texture from framegrabber.
            Screen('DrawTexture', win, tex, [], dstRect);
            
            % Print pts:
            Screen('DrawText', win, sprintf('%.4f', pts), 0, 0, 255);
            if count>0
                % Compute delta:
                delta = (pts - oldpts) * 1000;
                oldpts = pts;
                Screen('DrawText', win, sprintf('%.4f', delta), 0, 20, 255);
            end;
            
            % Show it.
            Screen('Flip', win);
            Screen('Close', tex);
        end
        count = count + 1;
    end
    telapsed = GetSecs - t %#ok<NOPRT>
    
    Screen('StopVideoCapture', grabber);
    Screen('CloseVideoCapture', grabber);
    
    Screen('CloseAll');
    avgfps = count / telapsed %#ok<NOPRT,NASGU>
    
catch %#ok<CTCH>
    Screen('CloseAll');
end
