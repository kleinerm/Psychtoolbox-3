function palowlatency(exactstart, deviceid)

% Establish key mapping: ESCape aborts, Space toggles between auto-
% movement of sound source or user mouse controlled movement:
KbName('UnifyKeynames');
space = KbName('space');
esc = KbName('ESCAPE');

dummy=GetSecs;
WaitSecs(0.1);

if nargin < 1 || isempty(exactstart)
    exactstart = 1;
end
exactstart

if nargin < 2 || isempty(deviceid)
    deviceid = -1;
end

deviceid

reqlatencyclass = 2; % class 2 empirically the best, 3 & 4 == 2
freq = 96000;        % Must set this. 96khz, 48khz, 44.1khz.
buffersize = [];     % Pointless to set this. Auto-selected to be optimal.

% Take hardware delay of MacBookPro into account: Assign it as bias.
latbias = (30 / freq)

pahandle = PsychPortAudio('Open', deviceid, [], reqlatencyclass, freq, 2, buffersize);
prelat = PsychPortAudio('LatencyBias', pahandle, latbias)
postlat = PsychPortAudio('LatencyBias', pahandle)


%mynoise = randn(2,freq * 0.1);
mynoise(1,:) = 0.9 * MakeBeep(1000, 0.1, freq);
mynoise(2,:) = mynoise(1,:);
% Fill buffer with data:
PsychPortAudio('FillBuffer', pahandle, mynoise);

% Perform some preflip to heat up the pipe:
screenid = max(Screen('Screens'));
win = Screen('OpenWindow', screenid, 0);
ifi = Screen('GetFlipInterval', win);

% Wait for keypress.
while KbCheck; end;
KbWait;

% Realtime scheduling:
% Priority(MaxPriority(win));

for i=1:10

    [vbl1 visonset1]= Screen('Flip', win);

    % Prepare black white transition:
    Screen('FillRect', win, 255);
    Screen('DrawingFinished', win);

    %pause(0.6);

    if exactstart
        % Schedule start of audio at exactly the predicted visual
        % stimulus onset caused by the next flip command:
        PsychPortAudio('Start', pahandle, 1, visonset1 + ifi, 0);
    end

    % Ok, the next flip will do a black-white transition...
    [vbl visual_onset t1] = Screen('Flip', win, vbl1+0.002);

    if ~exactstart
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

Priority(0);

PsychPortAudio('Close');
Screen('CloseAll');

% Done. Bye.
return;
