function VideoMultiCameraCaptureDemo(deviceIds, syncmode, movieName)
% Demonstrate simple use of built-in video capture engine.
%
% VideoMultiCameraCaptureDemo([deviceIds=all][, syncmode=0][, movieName])
%
% VideoMultiCameraCaptureDemo captures simultaneously from all cameras
% connected to your computer, or a subset of cameras if it is specified
% in the optional vector 'deviceIds', and then shows their video feeds
% in individual Psychtoolbox windows.
%
% The optional 'syncmode' flag allows to select synchronization strategy
% for multi-cam capture: 0 = None, all free-running. 4 = Software sync,
% 8 = Firewire Bus-Sync, 16 = Hardware (TTL) trigger sync.
%
% The optional 'movieName' string, if provided, will enable video recording
% of each cameras video into a dedicated movie file, which consists of the
% movieName and a unique camera number.
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

escape = KbName('ESCAPE');
backKey = KbName('LeftArrow');
forwardKey = KbName('RightArrow');

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

if nargin < 3 || isempty(movieName)
    doVideoRecording = 0;
else
    doVideoRecording = 1;
end    

roi = [0 0 640 480];
depth = [3];
convMode = 4; % Mode 4 would be the correct mode for raw sensor -> rgb bayer conversion on Basler cameras.
bayerPattern = 0; % This would be the correct bayer pattern for Basler color cameras.
debayerMethod = 3; % Debayer algorithm: 0 = Fastest, ..., 3 = High quality, 4-7 = May or may not work.

% Set dropframes = 0 if multiple frames shall be recorded for sync timing checks, 1 otherwise:
dropframes = 0;

% Flags to use for video recording. 16 = Use multi-threaded recording.
if doVideoRecording
    captureFlags = 16;
    codec = ':CodecType=DEFAULTencoder'; % Use default codec.
    dropframes = 1; % Enable frame dropping for now.
else
    captureFlags = 0;
    codec = [];
end

% Set noMaster = 1 if all cams are externally hardware triggered slaves, 0 otherwise:
noMaster = 1;

% Always use a master, ie., noMaster false if not hardware synced.
if syncmode ~= 16
    noMaster = 0;
end

% We limit default framerate to 30 fps instead of auto-detected maximum
% as supported by a camera in order to limit the consumption of bus bandwidth.
% Otherwise we might run out of bandwidth with multiple connected cameras.
fps = 60;
maxTargetFrameCount = 600;

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
      deviceIds = [deviceIds, devs(i).DeviceIndex]; %#ok<AGROW>
    end
  end
end

if isempty(deviceIds)
    fprintf('Sorry, could not detect any suitable video cameras. Bye.\n');
    return;
end

try
    for i=1:length(deviceIds)
      % Open oncreen window for i'th camera:
      win(i) = PsychImaging('OpenWindow', screenid, 0, [0, 0, 650, 500], [], [], [], [], [], kPsychGUIWindow + kPsychGUIWindowWMPositioned); %#ok<AGROW>

      % Set text size for info text:
      Screen('TextSize', win(i), 24);

      % Open i'th camera:
      grabbers(i) = Screen('OpenVideoCapture', win(i), deviceIds(i), roi, depth, 64, [], codec, captureFlags, [], 8); %#ok<AGROW>

      % Multi-camera sync mode requested?
      if syncmode > 0
        if i == 1
            % First one is sync-master: Unless noMaster == 1, in which case there are only slaves.
            Screen('SetVideoCaptureParameter', grabbers(1), 'SyncMode', syncmode + 1 + noMaster);
            if Screen('SetVideoCaptureParameter', grabbers(1), 'SyncMode') ~= (syncmode + 1 + noMaster)
                error('Sync master camera does not support requested sync mode %i! Game over!', syncmode);
            end
        else
            % Others are sync-slaves:
            Screen('SetVideoCaptureParameter', grabbers(i), 'SyncMode', syncmode + 2);
            if Screen('SetVideoCaptureParameter', grabbers(i), 'SyncMode') ~= syncmode + 2
                % This slave does not support syncmode! Means the syncmode must be hardware sync,
                % as that is the only one which can be unsupported. Fall back to bus sync and switch
                % master into combined hw sync + bus sync mode:
                fprintf('Sync slave camera %i does not support requested sync mode %i!\n', grabbers(i), syncmode);

                % Hardware sync requested but unsupported by this slave?
                if (syncmode == 16) && ~noMaster
                    % Yes, workaround via additional bus-sync and hope for the best:
                    fprintf('Switching sync slave camera %i to bus sync as fallback. Master will emit bus-sync signals too.\n', grabbers(i));
                    % Set slave to bus sync:
                    Screen('SetVideoCaptureParameter', grabbers(i), 'SyncMode', 8 + 2);
                    % Set master to bus sync in addition to hw sync:
                    Screen('SetVideoCaptureParameter', grabbers(1), 'SyncMode', 16 + 8 + 1);
                end
            end
        end
      end

      % Use TriggerMode 0 "Start of exposure by trigger, duration of exposure by 'Shutter' setting". Trigger exposure on falling edge,
      % (active low TriggerPolarity), get signal from TriggerSource 0, aka port 0:
      fprintf('Camera %i : OldTriggerMode = %i\n', grabbers(i), Screen('SetVideoCaptureParameter', grabbers(i), 'TriggerMode', 0));
      fprintf('Camera %i : OldTriggerPolarity = %i\n', grabbers(i), Screen('SetVideoCaptureParameter', grabbers(i), 'TriggerPolarity', 0));
      fprintf('Camera %i : OldTriggerSource = %i\n', grabbers(i), Screen('SetVideoCaptureParameter', grabbers(i), 'TriggerSource', 0));
      fprintf('Camera %i : TriggerSources = ', grabbers(i)); disp(Screen('SetVideoCaptureParameter', grabbers(i), 'GetTriggerSources'));
      
      % Configure i'th camera:
      %brightness = Screen('SetVideoCaptureParameter', grabber, 'Brightness',383)
      %exposure = Screen('SetVideoCaptureParameter', grabber, 'Exposure',130)
      %gain = Screen('SetVideoCaptureParameter', grabber, 'Gain')
      %gamma = Screen('SetVideoCaptureParameter', grabber, 'Gamma')
      %shutter = Screen('SetVideoCaptureParameter', grabber, 'Shutter', 7)
      %vendor = Screen('SetVideoCaptureParameter', grabber, 'GetVendorname')
      %model  = Screen('SetVideoCaptureParameter', grabber, 'GetModelname')
      %fps  = Screen('SetVideoCaptureParameter', grabber, 'GetFramerate')
      %roi  = Screen('SetVideoCaptureParameter', grabber, 'GetROI')
      Screen('SetVideoCaptureParameter', grabbers(i), 'PrintParameters')

      % Select convMode as conversion mode:
      Screen('SetVideoCaptureParameter', grabbers(i), 'DataConversionMode', convMode);

      % Set an override bayer pattern to use if it can't get auto-detected:
      Screen('SetVideoCaptureParameter', grabbers(i), 'OverrideBayerPattern', bayerPattern);
      
      % Select debayer algorithm:
      Screen('SetVideoCaptureParameter', grabbers(i), 'DebayerMethod', debayerMethod);

      % Ask engine to prefer Format-7 video capture modes over bog-standard Non-Format-7 ones.
      % Normally the engine decides itself what is the best choice. Forcing the engine to
      % use Format-7 sometimes allows to save a bit of bus-bandwidth and thereby squeeze out
      % higher framerates, resolutions or use of more cameras on a single bus in multi-cam capture.
      Screen('SetVideoCaptureParameter', grabbers(i), 'PreferFormat7Modes', 1);

      if doVideoRecording
            % If video recording is requested, set a unique movie filename per camera:
            Screen('SetVideoCaptureParameter', grabbers(i), sprintf('SetNewMoviename=%s_Cam%02d.mov', movieName, grabbers(i)));
      end
    end

    % Start capture on all cameras:
    if syncmode == 0
        % Start one after the other:
        for grabber=grabbers
            Screen('StartVideoCapture', grabber, fps, dropframes);
        end
    else
        % Multicam sync enabled. First start all slaves:
        oldverb = Screen('Preference','Verbosity', 4);
        fprintf('Prepare for synced start! Warp 1 ...\n\n');
        for grabber=grabbers(2:end)
            Screen('StartVideoCapture', grabber, fps, dropframes);
        end

        % Only wait for keypress if there is a designated master. With
        % only slaves, that wait would be futile:
        if ~noMaster
            KbStrokeWait;
        end
        fprintf('Engage! ...\n\n');

        % Start master:
        Screen('StartVideoCapture', grabbers(1), fps, dropframes);
        fprintf('And we are flying!\n\n');
        Screen('Preference','Verbosity', oldverb);
    end

    % If a specific maximum framecount is requested for end of capture/recording, set it here:
    if maxTargetFrameCount > 0
        % For maximum robustness one should set the count for all slaves first, before the master:
        for grabber = grabbers(2:end)
            Screen('SetVideoCaptureParameter', grabber, 'StopAtFramecount', maxTargetFrameCount);
        end

        % Finally set master / first grabber:
        Screen('SetVideoCaptureParameter', grabbers(1), 'StopAtFramecount', maxTargetFrameCount);
        fprintf('Targetting stop of capture at target framecount %i.\n', maxTargetFrameCount);
    end
    
    dstRect = [];
    count = 0;
    oldpts = zeros(size(grabbers));
    texcount = zeros(size(grabbers));
    camtex = zeros(length(grabbers), fps * 10);
    campts = zeros(size(camtex));
    t=GetSecs;

    % Multicapture and display loop: Runs until timeout of 120 seconds or until keypress
    % if there isn't any master cam or if no recording is requested:
    while dropframes || noMaster || ((GetSecs - t) < 120)
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
                if dropframes
                    Screen('Close', tex);
                else
                    texcount(i) = texcount(i) + 1;
                    camtex(i, texcount(i)) = tex;
                    campts(i, texcount(i)) = pts;
                end

                tex=0;
            end
            count = count + 1;
        end
    end
    
    telapsed = GetSecs - t;

    for grabber=grabbers
        fprintf('Camera %i : Bandwidth = %f\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'GetBandwidthUsage'));
        fprintf('Camera %i : UsedTriggerMode = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'TriggerMode'));
        fprintf('Camera %i : UsedTriggerPolarity = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'TriggerPolarity'));
        fprintf('Camera %i : UsedTriggerSource = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'TriggerSource'));
    end
    
    % Stop and shutdown all cameras:
    % Start capture on all cameras:
    if syncmode == 0
        % Stop one after the other:
        for grabber=grabbers
            Screen('StopVideoCapture', grabber);
        end
    else
        % Multicam sync enabled.

        % First some feedback about sync:
        for grabber=grabbers
            fprintf('Camera %i : Possible max framecount = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'GetFutureMaxFramecount'));
        end
    
        % Stop master first:
        fprintf('Drop us out of warp!\n');
        Screen('StopVideoCapture', grabbers(1));

        % Then stop all slaves:
        fprintf('Sublight reached ...\n');
        for grabber=grabbers(2:end)
            Screen('StopVideoCapture', grabber);
        end

        fprintf('And quarter impulse ahead.\n');
    end

    % Close down cameras, and some final stats:
    for grabber=grabbers
      fprintf('Camera %i : Final framecount = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'GetCurrentFramecount'));
      fprintf('Camera %i : Possible framecount = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'GetFutureMaxFramecount'));
      Screen('CloseVideoCapture', grabber);
    end

    if ~dropframes
        fi = 1;
        while 1
            [secs, keyCode] = KbWait; %#ok<ASGLU>
            if keyCode(escape)
                break;
            end

            if keyCode(backKey)
                fi = max(1, fi - 1);
            end

            if keyCode(forwardKey)
                fi = min(min(texcount), fi + 1);
            end

            for i=1:length(grabbers)
                % Draw new texture from framegrabber.
                Screen('DrawTexture', win(i), camtex(i, fi), [], dstRect);

                % Print pts:
                Screen('DrawText', win(i), sprintf('%.4f', campts(i, fi) - t), 0, 0, [1 0 0]);

                % Show it:
                Screen('Flip', win(i), [], [], 1);
            end
        end
    end
    
    % Close all windows and release all remaining display resources:
    Screen('CloseAll');
    Screen('Preference', 'SkipSyncTests', oldsynctests);
catch %#ok<*CTCH>
    % Emergency shutdown:
    sca;
    Screen('Preference', 'SkipSyncTests', oldsynctests);
    psychrethrow(psychlasterror);
end
