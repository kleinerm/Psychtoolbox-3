function VideoMultiCameraCaptureDemo(deviceIds, syncmode)
% Demonstrate simple use of built-in video capture engine.
%
% VideoMultiCameraCaptureDemo([deviceIds=all][, syncmode=0])
%
% VideoMultiCameraCaptureDemo captures simultaneously from all cameras
% connected to your computer, or a subset of cameras if it is specified
% in the optional vector 'deviceIds', and then shows their video feeds
% in individual Psychtoolbox windows.
%
% The optional 'syncmode' flag allows to select synchronization strategy
% for mult-cam capture: 0 = None, all free-running. 4 = Software sync,
% 8 = Firewire Bus-Sync, 16 = Hardware (TTL) trigger sync.
%
% By default, a capture rate of 30 frames per second at a resolution of
% 640 x 480 pixels is requested, and the timecode and interframe interval
% of each captured image is displayed in the top-left corner of each window.
% A press of the ESCape key ends the demo. The demo also ends automatically
% after a timeout of 10 minutes is reached.

% History:
% 04-Nov-2013	mk  Written.

if IsWin
    error('Sorry, this demo is not supported on MS-Windows, as it needs the DC1394 video capture engine.');
end

% Default init and setup:
PsychDefaultSetup(2);

if nargin < 1
    deviceIds=[];
end

if nargin < 2 || isempty(syncmode)
    syncmode = 0;
else
    if ~ismember(syncmode, [0,4,8,16])
        error('Invalid syncmode! Must be 0, 4, 8 or 16!');
    end
end

roi = [];
depth = [1];

% We limit default framerate to 30 fps instead of auto-detected maximum
% as supported by a camera in order to limit the consumption of bus bandwidth.
% Otherwise we might run out of bandwidth with multiple connected cameras.
fps = 30;

% For now we only use the DC1394-Firewire capture engine, as setup with
% the GStreamer engine is not impossible, but more difficult/error-prone
% and less flexible when it comes to complex camera control and sync.
Screen('Preference', 'DefaultVideocaptureEngine', 1);

% Also disable sync tests, as visual timing doesn't work well anyway
% on windowed non-fullscreen GUI windows:
oldsynctests = Screen('Preference', 'SkipSyncTests', 2);

screenid=max(Screen('Screens'));

if isempty(deviceIds)
  devs = Screen('VideoCaptureDevices');
  for i=1:length(devs)
    % Use device if either its deviceIndex is not zero, or all enumerated devices if
    % the libdc1394 capture engine is in use. On other engines, deviceIndex 0 is special
    % in that it defines the default capture device. The same device shows up a second
    % time under a distinct device index, so we have to filter out deviceIndex 0 to avoid
    % opening the same camera twice:
    if (devs(i).DeviceIndex ~= 0) || (Screen('Preference', 'DefaultVideocaptureEngine') == 1)
      disp(devs(i));
      deviceIds = [deviceIds, devs(i).DeviceIndex];
    end
  end
end

try
    for i=1:length(deviceIds)
      % Open oncreen window for i'th camera:
      win(i) = PsychImaging('OpenWindow', screenid, 0, [0, 0, 650, 500], [], [], [], [], [], kPsychGUIWindow);

      % Set text size for info text:
      Screen('TextSize', win(i), 24);

      % Open i'th camera:
      grabbers(i) = Screen('OpenVideoCapture', win(i), deviceIds(i), roi, depth, 16)

      % Multi-camera sync mode requested?
      if syncmode > 0
        if i == 1
            % First one is sync-master:
            Screen('SetVideoCaptureParameter', grabbers(i), 'SyncMode', syncmode + 1);
        else
            % Others are sync-slaves:
            Screen('SetVideoCaptureParameter', grabbers(i), 'SyncMode', syncmode + 2);
        end
      end
      
      % Configure i'th camera:
      %brightness = Screen('SetVideoCaptureParameter', grabber, 'Brightness',383)
      %exposure = Screen('SetVideoCaptureParameter', grabber, 'Exposure',130)
      %gain = Screen('SetVideoCaptureParameter', grabber, 'Gain')
      %gamma = Screen('SetVideoCaptureParameter', grabber, 'Gamma')
      %shutter = Screen('SetVideoCaptureParameter', grabber, 'Shutter', 7)
      %Screen('SetVideoCaptureParameter', grabber, 'PrintParameters')
      %vendor = Screen('SetVideoCaptureParameter', grabber, 'GetVendorname')
      %model  = Screen('SetVideoCaptureParameter', grabber, 'GetModelname')
      %fps  = Screen('SetVideoCaptureParameter', grabber, 'GetFramerate')
      %roi  = Screen('SetVideoCaptureParameter', grabber, 'GetROI')
    end

    % Start capture on all cameras:
    if syncmode == 0
        % Start one after the other:
        for grabber=grabbers
            Screen('StartVideoCapture', grabber, fps, 1);
        end
    else
        % Multicam sync enabled. First start all slaves:
        oldverb = Screen('Preference','Verbosity', 4);
        fprintf('Prepare for synced start! Warp 1 ... ');
        for grabber=grabbers(2:end)
            Screen('StartVideoCapture', grabber, fps, 1);
        end

        fprintf('Engage! ... ');

        % Start master:
        Screen('StartVideoCapture', grabbers(1), fps, 1);
        fprintf('And we are flying!\n');
        Screen('Preference','Verbosity', oldverb);
    end
    
    dstRect = [];
    count = 0;
    oldpts = zeros(size(grabbers));
    t=GetSecs;

    % Multicapture and display loop: Runs until timeout of 10 minutes or keypress:
    while (GetSecs - t) < 600
        if KbCheck
            break;
        end
        
        for i=1:length(grabbers)
	    [tex pts nrdropped] = Screen('GetCapturedImage', win(i), grabbers(i), 0);
	    % fprintf('tex = %i  pts = %f nrdropped = %i\n', tex, pts, nrdropped);
	    
	    if tex > 0
		% Perform first-time setup of transformations, if needed:
		%if fullsize && (count == 0)
		%    texrect = Screen('Rect', tex);
		%    winrect = Screen('Rect', win);
		%    sf = min([RectWidth(winrect) / RectWidth(texrect) , RectHeight(winrect) / RectHeight(texrect)]);
		%    dstRect = CenterRect(ScaleRect(texrect, sf, sf) , winrect);
		%end

		% Draw new texture from framegrabber.
		Screen('DrawTexture', win(i), tex, [], dstRect);

		% Print pts:
		Screen('DrawText', win(i), sprintf('%.4f', pts - t), 0, 0, [1 0 0]);
		if count > 0
		    % Compute delta:
		    delta = (pts - oldpts(i)) * 1000;
		    Screen('DrawText', win(i), sprintf('%.4f', delta), 0, 20, [1 0 0]);
		end
		oldpts(i) = pts;

		% Show it. Don't sync to video refresh at all, as it would cause
		% performance degradation due to lockstep between window updates.
		Screen('Flip', win(i), [], [], 2);
		Screen('Close', tex);
		tex=0;
	    end
	    count = count + 1;
        end
    end
    
    telapsed = GetSecs - t;
    
    % Stop and shutdown all cameras:
    % Start capture on all cameras:
    if syncmode == 0
        % Stop one after the other:
        for grabber=grabbers
            Screen('StopVideoCapture', grabber);
        end
    else
        % Multicam sync enabled. Stop master first:
        fprintf('Drop us out of warp!\n');
        Screen('StopVideoCapture', grabbers(1));

        % Then stop all slaves:
        fprintf('Sublight reached ...\n');
        for grabber=grabbers(2:end)
            Screen('StopVideoCapture', grabber);
        end

        fprintf('And quarter impulse ahead.\n');
    end

    % Close down cameras:
    for grabber=grabbers
      Screen('CloseVideoCapture', grabber);
    end

    % Close all windows and release all remaining display resources:
    Screen('CloseAll');
    Screen('Preference', 'SkipSyncTests', oldsynctests);
catch
    % Emergency shutdown:
    sca;
    Screen('Preference', 'SkipSyncTests', oldsynctests);
    psychrethrow(psychlasterror);
end
