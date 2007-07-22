function BasicSoundFeedbackDemo(reqlatency)
% BasicSoundFeedbackDemo([reqlatency=0])
%
% Demonstrates very basic usage of the new Psychtoolbox sound driver
% PsychPortAudio() for audio feedback.
%
% Sound is captured from the default recording device and then immediately
% played back via the default output device. Waveform data of captured
% sound is also plotted to a Matlab figure window during feedback.
%
% By default, feedback is tried with minimum latency, but you can ask for a
% specific latency by providing the optional parameter 'reqlatency'.
%
% If you need low-latency, make sure to read "help InitializePsychSound"
% carefully or contact the forum.
%

% History:
% 07/20/2007 Written (MK)

% Running on PTB-3? Abort otherwise.
AssertOpenGL;

% Latency provided?
if nargin < 1
    reqlatency = [];
end

if isempty(reqlatency)
    reqlatency = 0;
end

duplex = 0;
lat =  0.05;
freq = 96000;
% Wait for release of all keys on keyboard:
while KbCheck; end;

% Perform low-level initialization of the sound driver:
InitializePsychSound(1);

PsychPortAudio('Verbosity', 10);

pushedoutput = [];

% Open the default audio device [], with mode 2 (== Only audio capture),
% and a required latencyclass of 2 == low-latency mode, as well as
% a frequency of freq Hz and 2 sound channels for stereo capture.
% This returns a handle to the audio device:
painput = PsychPortAudio('Open', [], 2, 2, freq, 2);

% Preallocate an internal audio recording  buffer with a capacity of 10 seconds:
PsychPortAudio('GetAudioData', painput, 10);

% Open default audio device [] for playback (mode 0), low latency, 48khz,
% stereo output:
paoutput = PsychPortAudio('Open', [], [], 2, freq, 2);
if duplex
    paoutput = painput;
    PsychPortAudio('FillBuffer', paoutput, zeros(2, freq * lat));
end

% Start audio capture immediately and wait for the capture to start.
% Return estimated timestamp of when the first sample hit the
% microphone/sound input jack. We set the number of 'repetitions' to zero,
% i.e. record until recording is manually stopped.
capturestart = PsychPortAudio('Start', painput, 0, 0, 1);

% Wait for at least lat msecs of sound data to become available:
availsecs = 0;
while availsecs < lat
    WaitSecs(0.001);
    s=PsychPortAudio('GetStatus', painput);
    availsecs = s.RecordedSecs;
end

% Quickly readout available sound and initialize sound output buffer with
% it:
[audiodata offset]= PsychPortAudio('GetAudioData', painput);
PsychPortAudio('FillBuffer', paoutput, audiodata);

if duplex == 0
    % Start the playback engine and wait for start:
    playbackstart = PsychPortAudio('Start', paoutput, 0, 0, 1);
end

% Now the playback engine should output the first 20 msecs of our sound,
% while the capture engine captures the next msecs...
expecteddelay = (playbackstart - capturestart) * 1000;
fprintf('Expected latency at least %f msecs.\n', expecteddelay);

% Feedback loop: Runs until keypress ...
while ~KbCheck
    % Sleep about lat/2 msecs to give the engines time to at least capture and
    % output lat/2 msecs worth of sound ...
    WaitSecs(lat/2);
    
    % Get new captured sound data ...
    [audiodata offset overrun]= PsychPortAudio('GetAudioData', painput);
    % ... and stream it into our output buffer:
    while size(audiodata, 2) > 0
        fetch = min(size(audiodata, 2), freq * lat/2);
        pushdata = audiodata(:, 1:fetch);
        audiodata = audiodata(:, fetch+1:end);
        PsychPortAudio('FillBuffer', paoutput, pushdata, 1);
        %pushedoutput = [pushedoutput pushdata];
    end
    
    % Done.
end

% Done. Stop the capture engine:
PsychPortAudio('Stop', painput, 1);

% Drain its capture buffer...
[audiodata offset]= PsychPortAudio('GetAudioData', painput);

% Stop the playback engine:
PsychPortAudio('Stop', paoutput, 1);

% Ok, done. Close engines and exit.
PsychPortAudio('Close');

%plot(pushedoutput(1,:));

% Done.
fprintf('Demo finished, bye!\n');

return;
