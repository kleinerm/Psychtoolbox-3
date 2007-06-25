function PsychPortAudioTimingTest(exactstart, deviceid)


% Force GetSecs and WaitSecs into memory to avoid latency later on:
dummy=GetSecs;
WaitSecs(0.1);

% If 'exactstart' wasn't provided, assume user wants to test exact sync of
% audio and video onset, instead of testing total onset latency:
if nargin < 1 || isempty(exactstart)
    exactstart = 1;
end
exactstart

% Default to auto-selected default output device if none specified:
if nargin < 2 || isempty(deviceid)
    deviceid = -1;
end

deviceid

% Request latency mode 2, which used to be the best one in our measurement:
reqlatencyclass = 2; % class 2 empirically the best, 3 & 4 == 2

% Requested output frequency, may need adaptation on some audio-hw:
freq = 96000;        % Must set this. 96khz, 48khz, 44.1khz.
buffersize = 64;     % Pointless to set this. Auto-selected to be optimal.

% Take hardware delay of MacBookPro into account: Assign it as bias.
% Needs to determined via measurement once for each piece of audio
% hardware:
latbias = (30 / freq)
%latbias = -0.001

% Initialize driver, request low-latency preinit:
InitializePsychSound(1);

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
        PsychPortAudio('Start', pahandle, 1, visonset1 + ifi + 0.00098, 0);
    end

    % Ok, the next flip will do a black-white transition...
    [vbl visual_onset t1] = Screen('Flip', win, vbl1+0.002);

    if ~exactstart
        % No test of scheduling, but of absolute latency: Start audio
        % playback immediately:
        audio_onset = PsychPortAudio('Start', pahandle, 1, 0, 0);
    end

    t2 = GetSecs;
    tstart = t2;

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

    WaitSecs(0.3);

    % Stop playback:
    PsychPortAudio('Stop', pahandle);

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
