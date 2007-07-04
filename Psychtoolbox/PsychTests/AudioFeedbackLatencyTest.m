function AudioFeedbackLatencyTest(trigger, nrtrials, freq, freqout)
% AudioFeedbackLatencyTest([trigger=0.1] [, nrtrials=10] [, freq=44100][, freqout=44100])
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
% Results on MacbookPro suggest that the method works, but with a not 100%
% accuracy, so its still better to use external measurement equipment to
% test!!!
%
% EARLY BETA CODE: DON'T USE OR USE WITH GREAT CAUTION!
%
% Optional parameters:
% 'trigger' = Trigger level for detection of sound onset in captured sound.
%
% 'ntrials' = Number of measurement trials to perform.
%
% 'freq' = Samplerate of capture device.
%
% 'freqout' = Samplerate of playback device.
%
% Obviously this test function can only be used in a very silent room!
%

% History:
% 06/30/2007 Written (MK)

% Running on PTB-3? Abort otherwise.
AssertOpenGL;

deviceid = []

if nargin < 1
    trigger = [];
end

if isempty(trigger)
    trigger = 0.1;
end

if nargin < 2
    nrtrials = [];
end

if isempty(nrtrials)
    nrtrials = 10;
end

if nargin < 3
    freq = [];
end

if isempty(freq)
    freq = 44100;
end

if nargin < 4
    freqout = [];
end

if isempty(freqout)
    freqout = 44100;
end

% Wait for release of all keys on keyboard:
while KbCheck; end;

% Perform basic initialization of the sound driver and request low-latency
% preinit:
InitializePsychSound(1);

PsychPortAudio('Verbosity', 6);

% Open the default audio device [], with mode 2 (== Only audio capture),
% and a required latencyclass of zero 2 == low-latency mode, as well as
% a frequency of 44100 Hz and 2 sound channels for stereo capture.
% This returns a handle to the audio device:
pahandlerec = PsychPortAudio('Open', deviceid, 2, 2, freq, 2);

% Open 2nd audio device for playback of our test signal with same settings
% otherwise:
pahandleout = PsychPortAudio('Open', deviceid, 1, 2, freqout, 2);

% Build 1khZ, 90% peak amplitude beep tone of 0.1 secs duration, suitable
% for playback at 'freq' Hz:
testsound = 0.9 * MakeBeep(1000, 0.1, freqout);

% Initialize sound output buffer with it:
PsychPortAudio('FillBuffer', pahandleout, [testsound; testsound]);

% Provide externally measured latency bias:
%PsychPortAudio('LatencyBias', pahandleout, 0.00137 + 0.000136);

% Measurement loop, runs nrtrials trials:
for i=0:nrtrials

    % Preallocate an internal audio recording  buffer with a capacity of 10 seconds:
    % We do this in the trial-loop instead of just once. This way, the
    % buffer gets reset to initial conditions.
    PsychPortAudio('GetAudioData', pahandlerec, 10);

    % Start audio capture immediately and wait for the capture to start.
    % Return estimated timestamp of when the first sample hit the
    % microphone/sound input jack. We set the number of 'repetitions' to zero,
    % i.e. record until recording is manually stopped.
    recstart = PsychPortAudio('Start', pahandlerec, 0, 0, 1);

    % Start scheduled playback of test sound in one second from now, one
    % repetition, wait for start, retrieve estimated onset timestamp t2:
    t1 = GetSecs + 1;
    t2 = PsychPortAudio('Start', pahandleout, 1, t1, 1);
    delta = t2 - t1;
    
    failed = 0;
    
    % Audiotrigger codee: Fetch audio data and check against threshold:
    level = 0;

    % Repeat as long as below trigger-threshold:
    while level < trigger
        % Fetch current audiodata:
        [audiodata offset]= PsychPortAudio('GetAudioData', pahandlerec);

        % Compute maximum signal amplitude in this chunk of data:
        if ~isempty(audiodata)
            level = max(abs(audiodata(1,:)));
        else
            level = 0;
        end

        % Below trigger-threshold?
        if level < trigger
            % Wait for a millisecond before next scan:
            WaitSecs(0.001);
        end
    end

    if i > 0
        % Determine roundtrip latency:
        rtl(i) = GetSecs - t1;
    end
    
    % Ok, last fetched chunk was above threshold!
    % Find exact location of first above threshold sample.
    idx = min(find(abs(audiodata(1,:)) >= trigger));

    if i > 0
        % Compute "real" latency, taking real starting time of recording and
        % offset of the triggersample in the buffer into account:
        dt(i) = (recstart + ((offset + idx - 1) / freq)) - t1;

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

    % Stop capture after one more second:
    WaitSecs(0.1);
    PsychPortAudio('Stop', pahandlerec);

    % Stop playback:
    outstatus = PsychPortAudio('GetStatus', pahandleout);
    PsychPortAudio('Stop', pahandleout);

    % Perform a last fetch operation to get all remaining data from the capture engine:
    recordedaudio = [recordedaudio PsychPortAudio('GetAudioData', pahandlerec)];
    recstatus = PsychPortAudio('GetStatus', pahandlerec);

    % Plot it, just for the fun of it:
    nrsamples = size(audiodata(:,idx:end), 2);
    plot(1:nrsamples, audiodata(1,idx:end), 'r', 1:nrsamples, audiodata(2,idx:end), 'b', 1:nrsamples, trigger, '-');
    drawnow;

    % Print the stats:
    if i > 0
        fprintf('%i : Estimated input latency %f ms : Diff real - scheduled %f ms. \n', i, 1000 * rtl(i), 1000 * dt(i));
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
fprintf('Mean roundtrip %f ms, stddev %f ms.\n', mean(rtl), std(rtl));
fprintf('Mean offset    %f ms, stddev %f ms.\n', mean(dt), std(dt));

recstatus
outstatus

% Done.
fprintf('Demo finished, bye!\n');

