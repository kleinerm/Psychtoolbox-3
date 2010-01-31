function PsychPortAudioDataPixxTimingTest(waitTime, exactstart, deviceid, latbias)
% PsychPortAudioDataPixxTimingTest([waitTime = 1][, exactstart=1] [, deviceid=-1] [, latbias=0])
%
% Test script for sound onset timing reliability and sound onset
% latency of the PsychPortAudio sound driver.
%
% This script configures the driver for low latency and high timing
% precision, then executes ten trials where it tries to emit a beep sound,
% starting in exact sync to a black-white transition on the display.
%
% You'll need measurement equipment to use this: A DataPixx device from
% VPixx technologies, connected to your computer via the USB connection
% cable. Also a connection between the line-out jack of your soundcard and
% the line-in jack of the DataPixx to transmit the sound data. The DataPixx
% will receive the audio output of PsychPortAudio/Your Soundcard, timestamp
% it and send the computed timing data to your computer via USB.
%
% Some parameters may need tweaking. Make sure you got the special driver
% plugin as described in 'help InitializePsychSound' for best results.
%
% This is early alpha code, expect some rough edges...
%
% Optional parameters:
%
% 'waitTime'   = Time to wait (in seconds) before playing sound. Defaults
%                to 1 second if omitted.
%
% 'exactstart' = 0 -- Start immediately, measure absolute latency.
%              = 1 -- Test accuracy of scheduled sound onset. (Default)
%
% 'deviceid'   = -1 -- Auto-select optimal device (Default).
%             >=0   -- Select specified output device. See
%                      PsychPortAudio('GetDevices') for a list of devices.
%
% 'latbias'    = Hardware inherent latency bias. To be determined by
%                measurement - allows to PA to correct for it if provided.
%                Unit is seconds. Defaults to zero on MS-Windows, defaults
%                to the expected bias of a Intel MacBookPro on OS/X.
%

nTrials = 10;

% Initialize driver, request low-latency preinit:
InitializePsychSound(1);

if ~IsLinux
    PsychPortAudio('Verbosity', 10);
end

% Force GetSecs and WaitSecs into memory to avoid latency later on:
GetSecs; WaitSecs(0.1);

if nargin < 1
    waitTime = [];
end

if isempty(waitTime)
    waitTime = 1;
end

% If 'exactstart' wasn't provided, assume user wants to test exact sync of
% audio and video onset, instead of testing total onset latency:
if nargin < 2
    exactstart = [];
end

if isempty(exactstart)
    exactstart = 1;
end

if exactstart
    fprintf('Will test accuracy of scheduled sound onset, i.e. how well the driver manages to\n');
    fprintf('emit sound at exactly the specified "when" deadline. Sound should start in exact\n');
    fprintf('sync with display black-white transition (or at least very close - < 1 msec off).\n');
    fprintf('The remaining bias can be corrected by providing the bias as "latbias" parameter\n');
    fprintf('to this script. Variance of sound onset between trials should be very low, much\n');
    fprintf('smaller than 1 millisecond on a well working system.\n\n');
else
    fprintf('Well test total latency for immediate start of sound playback, i.e., the "when"\n');
    fprintf('parameter is set to zero. The difference between display black-white transition\n');
    fprintf('and start of emitted sound will be the total system latency.\n\n');
end

% Default to auto-selected default output device if none specified:
if nargin < 3
    deviceid = [];
end

if isempty(deviceid)
    deviceid = -1;
end

if deviceid == -1
    fprintf('Will use auto-selected default output device. This is the system default output\n');
    fprintf('device in "normal" (=reliable but high latency) mode. In low-latency mode its the\n');
    fprintf('device with the lowest inherent latency on your system (as determined by some internal\n');
    fprintf('heuristic). If you are not satisfied with the results you may query the available devices\n');
    fprintf('yourself via a call to devs = PsychPortAudio(''GetDevices''); and provide the device index\n');
    fprintf('of a suitable device\n\n');
else
    fprintf('Selected the following output device (deviceid=%i) according to your spec:\n', deviceid);
    devs = PsychPortAudio('GetDevices');
    for idx = 1:length(devs)
        if devs(idx).DeviceIndex == deviceid
            break;
        end
    end
    disp(devs(idx));
end

% Request latency mode 2, which used to be the best one in our measurement:
% classes 3 and 4 didn't yield any improvements, sometimes they even caused
% problems.
reqlatencyclass = 2; % class 2 empirically the best, 3 & 4 == 2

% Requested output frequency, may need adaptation on some audio-hw:
freq = 44100;       % Must set this. 96khz, 48khz, 44.1khz.
buffersize = 0;     % Pointless to set this. Auto-selected to be optimal.
suggestedLatencySecs = [];

if IsWin
    suggestedLatencySecs = 0.015 %#ok<NOPRT>
end

% Needs to determined via measurement once for each piece of audio
% hardware:
if nargin < 4
    latbias = [];
end

if isempty(latbias)
    if IsOSX
        % Take hardware delay of MacBookPro into account: Assign it as bias.
        % The DAC delay of Intel HDA onboard audio on MacBookPro is 30 frames,
        % according to spec, so the delay should be 30/frequency seconds. This
        % was empirically found to be true on our test setup.
        latbias = (30 / freq);
        fprintf('No "latbias" provided. Auto-selected bias to be %f seconds, assuming Intel MacBookPro hardware with\n', latbias);
        fprintf('built-in Intel HDA audio output.\n');
    else
        % Unknown system: Assume zero bias. User can override with measured
        % values:
        fprintf('No "latbias" provided. Assuming zero bias. You''ll need to determine this via measurement for best results...\n');
        latbias = 0;
    end
end

% Open audio device for low-latency output:
pahandle = PsychPortAudio('Open', deviceid, [], reqlatencyclass, freq, 2, buffersize, suggestedLatencySecs);

% Tell driver about hardwares inherent latency, determined via calibration
% once:
prelat = PsychPortAudio('LatencyBias', pahandle, latbias) %#ok<NOPRT,NASGU>
postlat = PsychPortAudio('LatencyBias', pahandle) %#ok<NOPRT,NASGU>

%mynoise = randn(2,freq * 0.1);
% Generate some beep sound 1000 Hz, 0.1 secs, 90% amplitude:
mynoise(1,:) = 0.5 * MakeBeep(1000, 0.1, freq);
mynoise(2,:) = mynoise(1,:);

% Fill buffer with data:
PsychPortAudio('FillBuffer', pahandle, mynoise);

% Switch to realtime scheduling at maximum allowable Priority:
Priority(MaxPriority(0));

% Initialize audio capture subsystem of Datapixx:
% 96 KhZ sampling rate, Mono capture: Average across channels (0), Audio
% input is line-in (2), Gain is 1.0 (1):
DatapixxAudioKey('Open', 96000, 0, 2, 1);

% Check settings by printing them:
dpixstatus = Datapixx('GetMicrophoneStatus') %#ok<NOPRT,NASGU>

% Triggerlevel shall be 10% aka 0.1:
DatapixxAudioKey('TriggerLevel', 0.1);

% Wait for keypress.
fprintf('\n\nPress any key to start measurement.\n\n');
KbStrokeWait;

% nTrials measurement trials:
for i=1:nTrials
    % Start the playback engine with an infinite start deadline, ie.,
    % start hardware, but don't play sound:
    PsychPortAudio('Start', pahandle, 1, inf, 0);

    % Wait a bit, say 100 msecs, so hardware is certainly running and settled:
    WaitSecs(0.1);
    
    % Start audio capture on DataPixx now. Return true 'tStartBox'
    % timestamp of start in box clock time:
    tStartBox = DatapixxAudioKey('CaptureNow');
    
    if exactstart
        % Schedule start of audio at exactly 'waitTime' seconds ahead:
        PsychPortAudio('RescheduleStart', pahandle, GetSecs + waitTime, 0);
    else
        % No test of scheduling, but of absolute latency: Start audio
        % playback immediately:
        PsychPortAudio('RescheduleStart', pahandle, 0, 0);
    end

    if 0
        % Spin-Wait until hw reports the first sample is played...
        offset = 0;
        while offset == 0
            status = PsychPortAudio('GetStatus', pahandle);
            offset = status.PositionSecs;
            plat = status.PredictedLatency;
            fprintf('Predicted Latency: %6.6f msecs.\n', plat*1000);
            if offset>0
                break;
            end
            WaitSecs('YieldSecs', 0.001);
        end
    end
    
    % Retrieve true delay from DataPixx measurement and stop recording on the device:
    [audiodata, measuredAudioDelta] = DatapixxAudioKey('GetResponse', waitTime + 1, [], 1);

    % Compute expected delay based on audio onset time as predicted/measured by
    % PsychPortAudio:
    status = PsychPortAudio('GetStatus', pahandle);
    tPortAudio(i) = status.StartTime; %#ok<AGROW>
    tDataPixx(i)  = tStartBox + measuredAudioDelta; %#ok<AGROW>
    
    fprintf('Buffersize %i, xruns = %i, playpos = %6.6f secs.\n', status.BufferSize, status.XRuns, status.PositionSecs);

    if 0
        figure;
        plot(audiodata);
    end

    % Stop playback:
    PsychPortAudio('Stop', pahandle, 1);
end

% Remap Datapixx clock timestamps to Psychtoolbox GetSecs() timestamps:
tDataPixx = PsychDataPixx('BoxsecsToGetsecs', tDataPixx);

% Done: Back to normal scheduling:
Priority(0);

% Close Datapixx audio subsystem:
DatapixxAudioKey('Close');

% Close PsychPortAudio:
PsychPortAudio('Close');

fprintf('\n\n');
for i =1:nTrials
    audioDelta(i) = 1000 * (tDataPixx(i) - tPortAudio(i)); %#ok<AGROW>
    fprintf('%i. PsychPortAudio measured onset error is %6.6f msecs.\n', i, audioDelta(i));
end

% Discard 1st trial:
audioDelta = audioDelta(2:end);

fprintf('\nAvg error %6.6f msecs, Stddev %6.6f msecs, Range %6.6f msecs.\n\n', mean(audioDelta), std(audioDelta), range(audioDelta));

% Done. Bye.
return;
