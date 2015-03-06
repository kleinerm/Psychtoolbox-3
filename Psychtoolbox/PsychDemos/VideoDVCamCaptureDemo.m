function VideoDVCamCaptureDemo(fullscreen, fullsize, roi, depth, deviceId, moviename)
% Demonstrate simple use of built-in video capture engine with DV consumer cameras.
%
% VideoDVCamCaptureDemo([fullscreen=0][, fullsize=0][, roi][, depth][,deviceId=0][, moviename])
%
% NOTE: As of October 2014, DV video camera capture has not been tested at
% all on MS-Windows, as GStreamer-1 currently doesn't ship with video
% capture plugins for MS-Windows. On Apple OSX 10.9.5 Mavericks with our
% new GStreamer-1 video capture engine, video capture from DV did not work,
% neither with Psychtoolbox, nor with GStreamer command line tools or other
% independent open source video capture applications. Only Apples Facetime
% app managed to get a marginally useable video stream from the DV camera.
% Testing on Linux with this specific demo showed mixed results. However on
% Linux there is a slightly hacky way that makes it work somewhat reliably
% with the new backend by exposing the camera as a regular video source, so
% all regular video capture/recording/processing demos can be used for DV
% capture, without any need for special treatment like in this demo. Read
% the section below for further instructions for a reliable DV setup on
% Linux.
%
% VideoDVCamCaptureDemo initializes the first attached and supported DV
% firewire consumer camera, then shows its video image in a Psychtoolbox
% window. A press of the ESCape key ends the demo.
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
% 'roi' Set to [0 0 720 576] for a PAL-DV camera and [0 0 720 480] for a
% NTSC-DV camera. Default is PAL if omitted.
%
% 'deviceId' Device index of video capture device. Defaults to system
% default. You can also specify a gst-launch style string to define a
% videosource here. Or you can set the special string deviceId = 'X' so
% builtin spec strings suitable for each operating system will be used.
%
% 'moviename' Name string for selection of filename of a target movie file
% to which video should be recorded. Defaults to none,ie., no video
% recording.
%
% VideoDVCamCaptureDemo also allows you to test out video capture from
% special video sources other than Consumer-DV cameras, ie. sources which
% require use of a custom built GStreamer video source bin. You test such
% setups by specifying the bin spec-string and other parameters as parameters
% for the demo. As an example, the following call would try to capture a video
% stream that is encoded as H264 video and that is transmitted over the network
% via TCP-IP protocol, ie., a H264 video stream encapsulated in TCP:
%
% VideoDVCamCaptureDemo([], [], [0 0 320 240], 6, 'tcpclientsrc port=8554 host=localhost ! h264parse ! avdec_h264 name=ptbdvsource');
%
% This would receive the TCP stream from port 8554 on the machine with
% the name 'localhost' (the local machine). The stream would decode from
% H264 to color format 6 - YUV-I420 - with video frames of 320 x 240 pixels
% size. For testing purpose you could enter the following GStreamer command
% line into a terminal window to generate a test video stream and send it from
% the local machine 'localhost' on port 8554, as H264 encoded TCP-IP stream,
% then receive it via VideoDVCamCaptureDemo as shown above:
%
% gst-launch-1.0 videotestsrc horizontal-speed=5 ! x264enc tune="zerolatency" threads=1 ! video/x-h264,stream-format=byte-stream ! tcpserversink port=8554
%
% One application of such a custom setup can be seen in the discussion thread
% containing message #18807 on the Psychtoolbox forum. There the video source
% is an IP camera attached to a robot, streaming H264 video over the network for
% consumption by a machine running Psychtoolbox.
%
%
%
%
% Loopback setup on Linux for use with new GStreamer-1 video backend:
%
% This specific configuration was shown to work at least on Ubuntu 14.04
% LTS with a Sony PAL-DV firewire camera. After following the setup steps,
% demos like our standard VideoCaptureDemo, VideoRecordingDemo, ... worked
% without any special configuration or treatment of DV cameras.
%
% Here you need to install a Video4Linux2 loopback kernel module. It will
% allow to create virtual video sources, from which Psychtoolbox can
% read/capture/process record live video. Then some external application
% can feed video into those virtual sources. You then attach an external
% command line DV capture session as video source.
%
% 1. Install the package "v4l2loopback-dkms" to get the kernel module installed and
%    loaded. A "sudo apt-get install v4l2loopback-dkms" on Ubuntu 14.04-LTS
%    and later distributions should do the trick. The package is probably
%    also available on Debian, other Debian/Ubuntu derived distros etc. Or
%    you get the most recent version to compile and install from source
%    code from the homepage of the project:
%    https://github.com/umlaeute/v4l2loopback
%
% 2. You may or may not need to "sudo modprobe v4l2loopback" on first use.
%
% 3. Then you use a GStreamer video capture pipeline launched from a terminal
%    window to connect to your DV camera, capture live video and feed it
%    into the virtual video loopback device. An example launch line can
%    look like this:
%
%    gst-launch dv1394src ! dvdemux ! dvdec ! v4l2sink device=/dev/video0
%
%    This would make live video from the first connected DV camera
%    available on /dev/video0. See
%    https://github.com/umlaeute/v4l2loopback/wiki for more detailed
%    instructions.
%
%    If this doesn't work for you with GStreamer-1 you may need to install
%    good old GStreamer-0.10 in addition to the already installed
%    GStreamer-1 and instead use the gst-launch-0.10 command instead of the
%    gst-launch command to select for the old implementation.
%
% 4. Psychtoolbox video capture functions should now report and be able to
%    use a new virtual video capture device with a name like "Dummy video
%    device 0000" or some name defined by you. Psychtoolbox should be able
%    to video capture or record video from that device aka your DV video
%    camera.
%
%    The Wiki of v4l2loopback describes more elaborate setups, e.g., for
%    capturing from multiple video DV cameras.
%

% History:
% 27-Dec-2013  mk  Written.
% 26-Aug-2014  mk  Adapted to GStreamer-1.4.0+ backend.
% 19-Sep-2014  mk  Update instructions for GStreamer-1.x, drop dead code for GStreamer-1.
% 05-Oct-2014  mk  Clarify OSX doesn't work at all. Deuglify help text formatting a bit.
% 02-Mar-2015  mk  Add example use case "video streaming over TCP network" to help text.

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
          %capturebinspec = 'dv1394src ! dvdemux ! avdec_dvvideo name=ptbdvsource';
        end
        
        if IsWin
          % This should work on MS-Windows, as tested with a Sony PAL-DV camera:
          capturebinspec = 'dshowvideosrc ! dvdemux ! ffdec_dvvideo name=ptbdvsource';
          
          if Is64Bit
              warning('To our knowledge (as of August 2014, GStreamer-1.4.0) video capture is not yet\nsupported on MS-Windows, so this will likely fail.\n');
          end
        end
        
        if IsOSX
          % This should work on OSX, if at all. If this doesn't work then game-over (Untested):
          warning('To our knowledge (as of October 2014, GStreamer-1.4.0) video capture from DV cameras on OSX does not work correctly, so this will likely fail.\n');
          capturebinspec = 'avfvideosrc name=ptbdvsource';
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

    % We refrain from setting target fps here, as DV cameras do have a fixed fps. Instead
    % we use the "do what you think is right" 'realmax' joker.
    Screen('StartVideoCapture', grabber, realmax, 1);

    dstRect = [];
    oldpts = 0;
    count = 0;
    t=GetSecs;
    
    % Run for at most 600 seconds or until keypress:
    while (GetSecs - t) < 600
        if KbCheck(-1)
            break;
        end
        
        [tex pts nrdropped,intensity]=Screen('GetCapturedImage', win, grabber, 1); %#ok<ASGLU,NASGU>
        % fprintf('tex = %i  pts = %f nrdropped = %i\n', tex, pts, nrdropped);
        
        if tex > 0
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
            if count > 0
                % Compute delta:
                delta = (pts - oldpts) * 1000;
                oldpts = pts;
                Screen('DrawText', win, sprintf('%.4f', delta), 0, 20, 255);
            end
            
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
