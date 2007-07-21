function PsychPortAudioTimingTest(exactstart, deviceid, latbias, waitframes)
% PsychPortAudioTimingTest([exactstart=1] [, deviceid=-1] [, latbias=0] [, waitframes=1])
%
% Test script for sound onset timing reliability and sound onset
% latency of the PsychPortAudio sound driver.
%
% This script configures the driver for low latency and high timing
% precision, then executes ten trials where it tries to emit a beep sound,
% starting in exact sync to a black-white transition on the display.
%
% You'll need measurement equipment to use this: A photo-diode attached to
% the top-left corner of your CRT display, a microphone attached to your
% speakers, some oszillograph to record and measure the signals from the
% diode and microphone.
%
% Some parameters may need tweaking. Make sure you got the special driver
% plugin as described in 'help InitializePsychSound' for best results.
%
% This is early alpha code, expect some rough edges...
%
% Optional parameters:
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
% 'waitframes' = Time to wait (in video refresh intervals) before emitting beep + flash.
%                Defaults to a value of 1, may need to be set higher for
%                high latency sound hardware (where absolute latency > 1
%                video refresh duration).

% Initialize driver, request low-latency preinit:
InitializePsychSound(1);

PsychPortAudio('Verbosity', 10);

% Force GetSecs and WaitSecs into memory to avoid latency later on:
dummy=GetSecs;
WaitSecs(0.1);

% If 'exactstart' wasn't provided, assume user wants to test exact sync of
% audio and video onset, instead of testing total onset latency:
if nargin < 1 
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
if nargin < 2 
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

% Needs to determined via measurement once for each piece of audio
% hardware:
if nargin < 3 
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

if nargin < 4
    waitframes = [];
end

if isempty(waitframes)
    waitframes = 1;
end

waitframes

% Open audio device for low-latency output:
pahandle = PsychPortAudio('Open', deviceid, [], reqlatencyclass, freq, 2, buffersize);

% Tell driver about hardwares inherent latency, determined via calibration
% once:
prelat = PsychPortAudio('LatencyBias', pahandle, latbias)
postlat = PsychPortAudio('LatencyBias', pahandle)

%mynoise = randn(2,freq * 0.1);
% Generate some beep sound 1000 Hz, 0.1 secs, 90% amplitude:
mynoise(1,:) = 0.9 * MakeBeep(1000, 0.1, freq);
mynoise(2,:) = mynoise(1,:);

% Fill buffer with data:
PsychPortAudio('FillBuffer', pahandle, mynoise);

% Setup display:
screenid = max(Screen('Screens'));
win = Screen('OpenWindow', screenid, 0);
ifi = Screen('GetFlipInterval', win);

% Wait for keypress.
while KbCheck; end;
KbWait;

% Realtime scheduling:
% Priority(MaxPriority(win));

% Ten measurement trials:
for i=1:10

    % This flip clears the display to black and returns timestamp of black
    % onset:
    [vbl1 visonset1]= Screen('Flip', win);

    % Prepare black white transition:
    Screen('FillRect', win, 255);
    Screen('DrawingFinished', win);

    %pause(0.6);

    if exactstart
        % Schedule start of audio at exactly the predicted visual
        % stimulus onset caused by the next flip command:
        PsychPortAudio('Start', pahandle, 1, visonset1 + waitframes * ifi, 0);
    end

    % Ok, the next flip will do a black-white transition...
    [vbl visual_onset t1] = Screen('Flip', win, vbl1 + (waitframes - 0.5) * ifi);

    if ~exactstart
        % No test of scheduling, but of absolute latency: Start audio
        % playback immediately:
        PsychPortAudio('Start', pahandle, 1, 0, 0);
    end

    t2 = GetSecs;

    % Spin-Wait until hw reports the first sample is played...
    offset = 0;
    while offset == 0
        status = PsychPortAudio('GetStatus', pahandle);
        offset = status.PositionSecs;
        t3=GetSecs;
        plat = status.PredictedLatency;
        fprintf('Predicted Latency: %6.6f msecs.\n', plat*1000);
        if offset>0
            break;
        end
        WaitSecs(0.001);
    end

    audio_onset = status.StartTime;
    status.TotalCalls


    %fprintf('Expected visual onset at %6.6f secs.\n', visual_onset);
    %fprintf('Sound started between %6.6f and  %6.6f\n', t1, t2);
    %fprintf('Expected latency sound - visual = %6.6f\n', t2 - visual_onset);
    %fprintf('First sound buffer played at %6.6f\n', t3);
    fprintf('Flip delay = %6.6f secs.  Flipend vs. VBL %6.6f\n', vbl - vbl1, t1-vbl);
    fprintf('Delay start vs. played: %6.6f secs, offset %f\n', t3 - t2, offset);

    fprintf('Buffersize %i, xruns = %i, playpos = %6.6f secs.\n', status.BufferSize, status.XRuns, status.PositionSecs);
    fprintf('Screen    expects visual onset at %6.6f secs.\n', visual_onset);
    fprintf('PortAudio expects audio onset  at %6.6f secs.\n', audio_onset);
    fprintf('Expected audio-visual delay    is %6.6f msecs.\n', (audio_onset - visual_onset)*1000.0);

    % Stop playback:
    PsychPortAudio('Stop', pahandle, 1);

    WaitSecs(0.3);

    Screen('FillRect', win, 0);
    telapsed = Screen('Flip', win) - visual_onset;
    WaitSecs(0.6);

end

% Done, close driver and display:
Priority(0);

PsychPortAudio('Close');
Screen('CloseAll');

% Done. Bye.
return;
