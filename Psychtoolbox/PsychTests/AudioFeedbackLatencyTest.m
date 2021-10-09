function AudioFeedbackLatencyTest(roundtrip, trigger, deviceid, nrtrials, freq, freqout, fullduplex, runmode)
% AudioFeedbackLatencyTest([roundtrip=0][, trigger=0.1][, deviceid=auto][, nrtrials=10][, freq=auto][, freqout=auto][, fullduplex=0][, runmode=1])
%
% Tries to test sound onset accuracy of PsychPortAudio without need for
% external measurement equipment: Sound signals are played back via
% PsychPortAudio at well defined points in time, using low-latency mode. At
% the same time, sound is captured via PsychPortAudio's capture facilities.
% The idea is that the microphone or line-in connector should pick up and
% capture the sound signals emitted through line-out (via a line-out ->
% line-in feedback cable) or emitted through the speakers. We measure and
% compare timing of emitted vs. captured sound spikes.
%
% Results on MacbookPro, Windows, Linux, suggest that the method works, not with
% 100% accuracy, so its still better to use external measurement equipment to test!
%
% EARLY BETA CODE: USE ONLY WITH GREAT CAUTION AND SUSPICION!
%
% Optional parameters:
%
% 'roundtrip' If set to 0 then this measures scheduling accuracy of sound
% onset, as measured by sound capture -- should be no worse than ~ 1 msec
% on a well working system, and input detection latency, ie., how long does
% it take from physical sound onset to detection of sound onset by the
% script. This would be a useful measure of how fast a "Voicekey" could
% respond to voice onset in a best case scenario.
%
% If set to 1 then this measures time from issuing the PsychPortAudio('Start')
% command to start playback until actual start of playback (by driver self-
% assessment, and by measuring via audio capture), and also as "Roundtrip"
% how long it would take to detect the onset by the script.
%
% 'trigger' = Trigger level for detection of sound onset in captured sound.
%
% 'deviceid' = Index of audio in/out device, if one device is used. If omitted,
% the default audio device is chosen. You can also specify a vector of two device
% indices, to specify separate input and output devices deviceid = [input, output].
% With different devices, full-duplex mode is obviously not supported.
%
% 'ntrials' = Number of measurement trials to perform.
%
% 'freq' = Samplerate of capture device.
%
% 'freqout' = Samplerate of playback device.
%
% 'fullduplex' = Use soundcard in full-duplex mode.
%
% 'runmode' = Runmode for PsychPortAudio to choose.
%
% Obviously this test function can only be used in a very silent room!
%

% History:
% 06/30/2007 Written (MK)
% 06/17/2020 Rewritten (MK)
% 10/08/2021 Improved: Auto-detect and handle different number of channels, frequency.
%            Allow different devices for input and output. (MK)

% Running on PTB-3? Abort otherwise.
AssertOpenGL;

if nargin < 1 || isempty(roundtrip)
    roundtrip = 0;
end

if nargin < 2 || isempty(trigger)
    trigger = 0.1;
end

if nargin < 3 || isempty(deviceid)
    indeviceid = [];
    outdeviceid = [];
else
    if length(deviceid) < 2
        indeviceid = deviceid;
        outdeviceid = deviceid;
    else
        indeviceid = deviceid(1);
        outdeviceid = deviceid(2);
    end
end

if nargin < 4 || isempty(nrtrials)
    nrtrials = 10;
end

if nargin < 5 || isempty(freq)
    freq = [];
end

if nargin < 6 || isempty(freqout)
    freqout = [];
end

if nargin < 7 || isempty(fullduplex)
   fullduplex = 0;
end

if nargin < 8 || isempty(runmode)
   runmode = 1;
end

if ~isempty(indeviceid) && (indeviceid ~= outdeviceid) && fullduplex
    fullduplex = 0;
    fprintf('Warning: Different input and output audio device, so force-disabling requested full-duplex.\n');
end

fprintf('Using runmode %i.\n', runmode);

% Wait for release of all keys on keyboard:
KbReleaseWait;

% Perform basic initialization of the sound driver and request low-latency
% preinit:
InitializePsychSound(1);

PsychPortAudio('Verbosity', 6);

if fullduplex
   % Open the default audio device indeviceid, with mode 3 (== Full-Duplex),
   % and a required latencyclass of 2 == agressive low-latency mode, as well as
   % a frequency of freq Hz and auto sound channel for capture.
   % This returns a handle to the audio device:
   pahandlerec = PsychPortAudio('Open', indeviceid, 3, 2, freq);
   pahandleout = pahandlerec;
else
   % Open the default audio device indeviceid, with mode 2 (== Only audio capture),
   % and a required latencyclass of 2 == agressive low-latency mode, as well as
   % a frequency of freq Hz and auto sound channel for capture.
   % This returns a handle to the audio device:
   pahandlerec = PsychPortAudio('Open', indeviceid, 2, 2, freq);

   % Open 2nd audio device for playback of our test signal with same settings
   % otherwise:
   pahandleout = PsychPortAudio('Open', outdeviceid, 1, 2, freqout);
end

PsychPortAudio('RunMode', pahandlerec, runmode);
PsychPortAudio('RunMode', pahandleout, runmode);

% Find number of output channels and playback frequency:
status = PsychPortAudio('GetStatus', pahandleout);
freqout = status.SampleRate;
props = PsychPortAudio('GetDevices', [], status.OutDeviceIndex);
outChannels = min(2, props.NrOutputChannels);

% Find number of input channels and capture frequency:
status = PsychPortAudio('GetStatus', pahandlerec);
freq = status.SampleRate;
props = PsychPortAudio('GetDevices', [], status.InDeviceIndex);
inChannels = min(2, props.NrInputChannels);

% Build 1khZ, 90% peak amplitude beep tone of 0.1 secs duration, suitable
% for playback at 'freqout' Hz:
testsound = 0.9 * MakeBeep(1000, 0.1, freqout);
testsound = repmat(testsound, outChannels, 1);

% Initialize sound output buffer with it:

PsychPortAudio('FillBuffer', pahandleout, testsound);

% Measurement loop, runs nrtrials trials:
for i = 0:nrtrials
    % Preallocate an internal audio recording  buffer with a capacity of 10 seconds:
    % We do this in the trial-loop instead of just once. This way, the
    % buffer gets reset to initial conditions.
    PsychPortAudio('GetAudioData', pahandlerec, 10);

    % Start audio capture immediately and wait for the capture to start.
    % Return estimated timestamp of when the first sample hit the
    % microphone/sound input jack. We set the number of 'repetitions' to zero,
    % i.e. record until recording is manually stopped.
    if ~fullduplex
       recstart = PsychPortAudio('Start', pahandlerec, 0, 0, 1);
    end

    % Start scheduled playback of test sound in one second from now, one
    % repetition, wait for start, retrieve true estimated onset timestamp t1:
    t1 = GetSecs + 1;

    if roundtrip
        t1 = WaitSecs('UntilTime', t1);
        t2 = PsychPortAudio('Start', pahandleout, 1, 0, 1);
    else
        t1 = PsychPortAudio('Start', pahandleout, 1, t1, 1);
    end
    failed = 0;

    % Audiotrigger code: Fetch audio data and check against threshold:
    level = 0;

    % Repeat as long as below trigger-threshold:
    while level < trigger
        % Fetch current audiodata:
        [audiodata, offset, ~, recstart]= PsychPortAudio('GetAudioData', pahandlerec);

        % Compute maximum signal amplitude in this chunk of data:
        if ~isempty(audiodata)
            level = max(max(abs(audiodata)));
        else
            level = 0;
        end

        % Below trigger-threshold?
        if level < trigger
            % fprintf('Level %f < Trigger %f\n', level, trigger);
            % Wait for a millisecond before next scan:
            WaitSecs('YieldSecs', 0.001);
        end
    end

    if i > 0
        % Determine roundtrip or input latency:
        rtl(i) = GetSecs - t1; %#ok<AGROW>
    end

    % Ok, last fetched chunk was above threshold!
    % Find exact location of first above threshold sample.
    idx = find(max(abs(audiodata)) >= trigger, 1);

    if i > 0
        % Compute "real" latency, taking real starting time of recording and
        % offset of the triggersample in the buffer into account:
        dt(i) = (recstart + ((offset + idx - 1) / freq)) - t1; %#ok<AGROW>

        if dt(i) < -0.1
            % Invalid measurement! Abort whole procedure...
            fprintf('Trial %i: INVALID MEASUREMENT %f secs detected (Value %i -> %i := %f). Aborting. Please raise the trigger threshold and retry.\n', i, dt(i), offset + idx, idx, audiodata(1,idx));
            failed = 1;
            break;
        end
    end

    % Initialize our recordedaudio vector with captured data starting from
    % triggersample:
    recordedaudio = audiodata(:, idx:end);

    % Stop capture after a tiny bit more time:
    WaitSecs(0.1);
    if ~fullduplex
       PsychPortAudio('Stop', pahandlerec);
    end

    % Stop playback:
    outstatus = PsychPortAudio('GetStatus', pahandleout); %#ok<NASGU>
    PsychPortAudio('Stop', pahandleout);

    % Perform a last fetch operation to get all remaining data from the capture engine:
    recordedaudio = [recordedaudio PsychPortAudio('GetAudioData', pahandlerec)]; %#ok<AGROW,NASGU>
    recstatus = PsychPortAudio('GetStatus', pahandlerec); %#ok<NASGU>

    % Plot it, just for the fun of it:
    nrsamples = size(audiodata(:,idx:end), 2);
    if inChannels >= 2
        plot(1:nrsamples, audiodata(1,idx:end), 'r', 1:nrsamples, audiodata(2,idx:end), 'b', 1:nrsamples, repmat(trigger, 1, nrsamples), '-', 1:nrsamples, repmat(-trigger, 1, nrsamples), '-');
    else
        plot(1:nrsamples, audiodata(1,idx:end), 'r', 1:nrsamples, repmat(trigger, 1, nrsamples), '-', 1:nrsamples, repmat(-trigger, 1, nrsamples), '-');
    end
    drawnow;

    % Print the stats:
    if i > 0
        if roundtrip
            fprintf('%i : Estimated roundtrip latency %f ms : Measured output start to sound onset latency %f ms. Sound output delay %f ms.\n', i, 1000 * rtl(i), 1000 * dt(i), 1000 * (t2 - t1));
        else
            fprintf('%i : Estimated input latency %f ms : Diff real - scheduled %f ms. \n', i, 1000 * rtl(i), 1000 * dt(i));
        end
    end

    % Pause for a second, then next trial...
    WaitSecs(1);
end

% Close the audio devices:
PsychPortAudio('Close');

if failed
    return;
end

rtl = 1000 * rtl;
dt = 1000 * dt;
if roundtrip
    fprintf('Mean roundtrip latency             %f ms, stddev %f ms.\n', mean(rtl), std(rtl));
    fprintf('Mean startup to sound output delay %f ms, stddev %f ms.\n', mean(dt), std(dt));
else
    fprintf('Mean input latency     %f ms, stddev %f ms.\n', mean(rtl), std(rtl));
    fprintf('Mean scheduling offset %f ms, stddev %f ms.\n', mean(dt), std(dt));
end

% Done.
fprintf('Demo finished, bye!\n');
