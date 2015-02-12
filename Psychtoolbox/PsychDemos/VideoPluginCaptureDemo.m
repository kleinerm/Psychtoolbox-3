function VideoPluginCaptureDemo(deviceIds, syncmode, movieName)
% Demonstrate simple use of built-in video marker tracking engine.
%
% EXPERIMENTAL CODE - NOT USEFUL FOR REGULAR PSYCHTOOLBOX USERS.
%
% VideoPluginCaptureDemo([deviceIds=all][, syncmode=0][, movieName])
%
% VideoPluginCaptureDemo captures simultaneously from all cameras
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
%
% This demo has various hard-coded parameters which allow you to tinker with
% things like Bayer filtering on the camera, versus on the host cpu, versus
% offline later on during playback of recorded video, to select different
% tradeoffs between quality, cpu load, speed and bus bandwidth use. It also
% allows to play with high bit depth (> 8 bpc) color and luminance formats,
% lossless video recording and other bits and pieces. Change the variables
% as you see fit and read the source carefully.
%

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

roi = [0 0 1280 960];
depth = []; % Choose a color depth of 1 if you want raw sensor data - or grayscale of course. 3 for RGB.
% Mode 4 would be the correct mode for raw sensor -> rgb bayer conversion on Basler cameras.
% Mode 3 would pass raw sensor data instead.
convMode = 4;
bayerPattern = 0; % This would be the correct bayer pattern for Basler color cameras.
debayerMethod = 3; % Debayer algorithm: 0 = Fastest, ..., 3 = High quality, 4-7 = May or may not work.

% Bitdepth to request from camera: 8 bpc is standard. 16 bpc requests anything between 9 bpc and 16 bpc:
bitdepth = 8;

% Set dropframes = 0 if multiple frames shall be recorded for sync timing checks, 1 otherwise:
dropframes = 1;

% Flags to use for video recording. 16 = Use multi-threaded recording.
if doVideoRecording
    captureFlags = 16;
    codec = ':CodecType=DEFAULTencoder UseVFR'; % Use default codec. UseVFR for variable framerate recording.
    
    % The huffyuv and ffenc_sgi codecs are lossless codecs. They use lossless compression, so video images
    % are stored exactly, but the resulting movie files will be comparably large!
    % These are mostly useful if you want to use the material for sensitive computer vision algorithms,
    % or if you want to store raw camera sensor data without Bayer filtering and perform the Bayer filtering
    % for raw sensor -> RGB conversion later on during playback. In that case you will need exact storage of
    % video and hence one of these codecs.
    %codec = ':CodecType=huffyuv UseVFR'; % Use HuffYUV Huffman encoded lossless codec. UseVFR for variable framerate recording.
    %codec = ':CodecType=ffenc_sgi UseVFR'; % Use SGI-RLE lossless codec. UseVFR for variable framerate recording.
    
    % For > 8 bpc recording, we need to use our own proprietary video encoding:
    % Note: Only use with lossless video codecs makes sense for 16 bpc mode.
    % Currently supported are huffyuv and ffenc_sgi.
    if bitdepth > 8
        codec = [codec ' UsePTB16BPC'];
    end
else
    captureFlags = 16;
    codec = [];
end

% Optional processing string for dropframes = 0 && captureFlags & 16 capture/recording:
processingString = '';

% Example processingString to apply an image warping effect:
%processingString = 'ffmpegcolorspace ! warptv ! ffmpegcolorspace ! capsfilter caps="video/x-raw-rgb, bpp=(int)24, depth=(int)24, endianess=(int)4321, red_mask=(int)16711680, green_mask=(int)65280, blue_mask=(int)255"';

% Set noMaster = 1 if all cams are externally hardware triggered slaves, 0 otherwise:
noMaster = 1;

% Always use a master, ie., noMaster false if not hardware synced.
if syncmode ~= 16
    noMaster = 0;
end

% We limit default framerate to 30 fps instead of auto-detected maximum
% as supported by a camera in order to limit the consumption of bus bandwidth.
% Otherwise we might run out of bandwidth with multiple connected cameras.
fps = 30;

% Select maximum number of frames to capture and record: Zero for "unlimited"
maxTargetFrameCount = 0;

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
        win(i) = PsychImaging('OpenWindow', screenid, 0, [0, 0, 1290, 1000], [], [], [], [], [], kPsychGUIWindow + kPsychGUIWindowWMPositioned); %#ok<AGROW>
        
        % Set text size for info text:
        Screen('TextSize', win(i), 24);
        
        % Open i'th camera:
        grabbers(i) = Screen('OpenVideoCapture', win(i), deviceIds(i), roi, depth, 64, [], codec, captureFlags, [], bitdepth); %#ok<AGROW>
        
        if i == 1
            % Reset firewire bus for this camera:
            % Screen('SetVideoCaptureParameter', grabbers(i), 'ResetBus');
            
            % Reset this camera:
            %Screen('SetVideoCaptureParameter', grabbers(i), 'ResetCamera');
        end
        
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
        %brightness = Screen('SetVideoCaptureParameter',  grabbers(i), 'Brightness',383)
        %exposure = Screen('SetVideoCaptureParameter',  grabbers(i), 'Exposure',130)
        %gain = Screen('SetVideoCaptureParameter',  grabbers(i), 'Gain')
        %gamma = Screen('SetVideoCaptureParameter',  grabbers(i), 'Gamma')
        %shutter = Screen('SetVideoCaptureParameter',  grabbers(i), 'Shutter', 7)
        %vendor = Screen('SetVideoCaptureParameter',  grabbers(i), 'GetVendorname')
        %model  = Screen('SetVideoCaptureParameter',  grabbers(i), 'GetModelname')
        %fps  = Screen('SetVideoCaptureParameter',  grabbers(i), 'GetFramerate')
        %roi  = Screen('SetVideoCaptureParameter',  grabbers(i), 'GetROI')
        
        %bmode = Screen('SetVideoCaptureParameter',  grabbers(i), '1394BModeActive', 0)
        %isospeed = Screen('SetVideoCaptureParameter',  grabbers(i), 'ISOSpeed', 400)
        
        %pio = Screen('SetVideoCaptureParameter',  grabbers(i), 'PIO', 1)
        %pio = Screen('SetVideoCaptureParameter',  grabbers(i), 'PIO')
        
        %[targetTemperature, currentTemperature] = Screen('SetVideoCaptureParameter',  grabbers(i), 'Temperature', 123)
        %[u,v] = Screen('SetVideoCaptureParameter',  grabbers(i), 'WhiteBalance', 161, 116)
        %[u,v] = Screen('SetVideoCaptureParameter',  grabbers(i), 'WhiteBalance')
        
        %[wr, wg, wb] = Screen('SetVideoCaptureParameter',  grabbers(i), 'WhiteShading')
        
        % Measure firewire bus cycles for fun and profit :-)
        [cycleSecs1, cycleSystemTime1, cycleSec, cycleCount, cycleOffset] = Screen('SetVideoCaptureParameter',  grabbers(i), 'GetCycleTimer');
        WaitSecs(0.1);
        [cycleSecs2, cycleSystemTime2] = Screen('SetVideoCaptureParameter',  grabbers(i), 'GetCycleTimer');
        fprintf('Camera %i: Bus runs at %f seconds per second.\n', grabbers(i), (cycleSecs2 - cycleSecs1) / (cycleSystemTime2 - cycleSystemTime1));
        fprintf('Camera %i: Initial bus timestamps were cycleSec %i, cycleCount %i, cycleOffset %i.\n', grabbers(i), cycleSec, cycleCount, cycleOffset);
        
        % Basler camera?
        if strfind(Screen('SetVideoCaptureParameter', grabbers(i), 'GetVendorname'), 'Basler')
            % Yes. Enable some Basler specific SFF features:
            
            % The Basler SFF framecounter is mostly useless to us, as it only resets to zero on a power-cycle,
            % and the cameras can't get power-cycled in software, only by physical unplug/replug :(.
            % Update: Some Basler cameras allow power-cycling, so it does work on those, e.g., the A602f works,
            % but the A312fc doesn't.
            % baslerFrameCounter = Screen('SetVideoCaptureParameter', grabbers(i), 'BaslerFrameCounterEnable')
            
            % Basler SFF CRC checksumming can find corrupted video frames due to problems with camera electronics,
            % bus cables, controllers, OS etc. It is very compute intense though, adds easily 10 msecs per frame,
            % so one should probably only enable it to diagnose real problems or initially verify a setup:
            % baslerChecksum = Screen('SetVideoCaptureParameter', grabbers(i), 'BaslerChecksumEnable')
            
            % Basler SFF timestamping can theoretically provide a capture timestamp corresponding to the exact
            % time when image exposure on the cameras sensor was started. The camera can tag frames with the
            % firewire bus time of start of exposure and Screen() can translate that into GetSecs time.
            %
            % In practice, all two tested models of Basler cameras didn't work. They returned a constant
            % bogus value as timestamp, leading to bogus capture timestamps. No bug could be found in our code
            % or the libdc1394 code when comparing code against the Basler SFF spec documents.
            % baslerTimestamp = Screen('SetVideoCaptureParameter', grabbers(i), 'BaslerFrameTimestampEnable')
        end
        
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
        %Screen('SetVideoCaptureParameter', grabbers(i), 'PreferFormat7Modes', 1);
        
        if doVideoRecording
            % If video recording is requested, set a unique movie filename per camera:
            Screen('SetVideoCaptureParameter', grabbers(i), sprintf('SetNewMoviename=%s_Cam%02d.mov', movieName, grabbers(i)));
        end
        
        % If a specific maximum framecount is requested for end of capture/recording, set it here
        % before capture gets started:
        % Setting the count for the slaves it not always neccessary, as the master will distribute
        % its limit to its slaves in many common scenarios, but then "better safe than sorry".
        if maxTargetFrameCount > 0
            Screen('SetVideoCaptureParameter', grabbers(i), 'StopAtFramecount', maxTargetFrameCount);
            fprintf('Targetting stop of capture on camera %i at target framecount %i.\n', grabbers(i), maxTargetFrameCount);
        end
        
        % Assign GStreamer post processing string, if any:
        if ~isempty(processingString)
            Screen('SetVideoCaptureParameter', grabbers(i), sprintf('SetGStreamerProcessingPipeline=%s', processingString));
        end
        
        % Load and setup markertracker plugin:
        pluginPath = [MFRoot 'MFBasic/Octave3LinuxFiles64/ptbvideomarkertrackerplugin.so'];
        Screen('SetVideoCaptureParameter', grabbers(i), sprintf('LoadMarkerTrackingPlugin=%s', pluginPath));
        Screen('SetVideoCaptureParameter', grabbers(i), sprintf('SendCommandToMarkerTrackingPlugin=CMD_TRACKERCAMID %i', 0));
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_UDPSTREAMTO localhost');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETDRAWMODE 15');

        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETDEBUGLEVEL 1');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETMINAREATHRESHOLD 10');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETMAXAREATHRESHOLD 1000');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETMININTENSITYTHRESHOLD 100');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETMAXINTENSITYTHRESHOLD 255');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETREDGREENMARKERGAIN 0.5');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETMATCHMETHOD 1');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_GLOBALMATCHINGSIGMA 1');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETMINELLIPSEASPECTTHRESHOLD 0.5');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETMAXELLIPSEAREATHRESHOLD 1000');
        Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=CMD_SETMARKERFEEDBACKCRITERION 0');
        %Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=');
        %Screen('SetVideoCaptureParameter', grabbers(i), 'SendCommandToMarkerTrackingPlugin=');
        
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
    
    % If a specific maximum framecount is requested for end of capture/recording, set it here dynamically
    % while capture is already started:
    if maxTargetFrameCount < 0
        % For maximum robustness one should set the count for all slaves first, before the master:
        % Setting the count for the slaves it not always neccessary, as the master will distribute
        % its limit to its slaves in many common scenarios, but then "better safe than sorry".
        for grabber = grabbers(2:end)
            Screen('SetVideoCaptureParameter', grabber, 'StopAtFramecount', abs(maxTargetFrameCount));
        end
        
        % Finally set master / first grabber:
        Screen('SetVideoCaptureParameter', grabbers(1), 'StopAtFramecount', abs(maxTargetFrameCount));
        fprintf('Targetting stop of capture at target framecount %i.\n', abs(maxTargetFrameCount));
    end
    
    capturestopped = 0;
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
            if ~capturestopped
                capturestopped = 1;
                maxTargetFrameCount = Screen('SetVideoCaptureParameter', grabbers(1), 'GetFutureMaxFramecount');
                fprintf('Logically stopping capture on master by setting stop frame count to current maximum framecount %i!\n', maxTargetFrameCount);
                Screen('SetVideoCaptureParameter', grabbers(1), 'StopAtFramecount', maxTargetFrameCount);
                fprintf('Capture on master scheduled to stop.\n');
                KbReleaseWait;
            else
                break;
            end
        end
        
        for i=1:length(grabbers)
            [tex pts nrdropped] = Screen('GetCapturedImage', win(i), grabbers(i), 0); %#ok<NASGU>
            % fprintf('tex = %i  pts = %f nrdropped = %i\n', tex, pts, nrdropped);
            
            if tex > 0
                % Draw new texture from framegrabber.
                Screen('DrawTexture', win(i), tex, [], dstRect);
                
                % Print pts:
                Screen('DrawText', win(i), sprintf('%.4f    [%i of %i]', pts - t, Screen('SetVideoCaptureParameter', grabbers(i), 'GetFetchedFramecount'), Screen('SetVideoCaptureParameter', grabbers(i), 'GetCurrentFramecount')), ...
                    0, 0, [1 0 0]);
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
                    texcount(i) = texcount(i) + 1; %#ok<UNRCH>
                    camtex(i, texcount(i)) = tex;
                    campts(i, texcount(i)) = pts;
                end
                tex = 0; %#ok<NASGU>
            end
            count = count + 1;
        end
        % WaitSecs('YieldSecs', 0.030);
    end
    
    telapsed = GetSecs - t;
    fprintf('Elapsed runtime %f seconds.\n', telapsed);
    
    for grabber=grabbers
        fprintf('Camera %i : Bandwidth = %f\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'GetBandwidthUsage'));
        fprintf('Camera %i : UsedTriggerMode = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'TriggerMode'));
        fprintf('Camera %i : UsedTriggerPolarity = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'TriggerPolarity'));
        fprintf('Camera %i : UsedTriggerSource = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'TriggerSource'));
    end
    
    % Stop all cameras:
    if syncmode == 0
        % Unsynchronized operation: Stop one after the other:
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
        fprintf('Camera %i : Corrupted frames count = %i\n', grabber, Screen('SetVideoCaptureParameter', grabber, 'GetCorruptFramecount'));
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
    
    % Close all textures explicitely to avoid "many open textures" warning:
    Screen('Close');
    
    % Close all windows and release all remaining display resources:
    Screen('CloseAll');
    Screen('Preference', 'SkipSyncTests', oldsynctests);
catch %#ok<*CTCH>
    % Emergency shutdown:
    sca;
    Screen('Preference', 'SkipSyncTests', oldsynctests);
    psychrethrow(psychlasterror);
end
