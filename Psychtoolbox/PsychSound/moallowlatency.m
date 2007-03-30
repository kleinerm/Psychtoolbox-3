function moallowlatency

% Establish key mapping: ESCape aborts, Space toggles between auto-
% movement of sound source or user mouse controlled movement:
KbName('UnifyKeynames');
space = KbName('space');
esc = KbName('ESCAPE');

% Initialize OpenAL subsystem at debuglevel 2 with the default output
% device:
InitializeMatlabOpenAL(0);
dummy=GetSecs;
WaitSecs(0.1);

% Generate one sound buffer:
buffers = alGenBuffers(1);

% Query for errors:
alGetString(alGetError)

% Create sound data:
freq = 44100;

% Start off with 10 seconds of 48 Khz random stereo noise as a fallback:
%mynoise = randn(2,freq * 0.1);

% Create sound data:
freq = 44100;

% Start off with 10 seconds of 48 Khz random stereo noise as a fallback:
%mynoise = randn(2,freq * 0.1);
mynoise(1,:) = 0.9 * MakeBeep(1000, 0.1, freq);
mynoise(2,:) = mynoise(1,:);

% Convert to 16 bit signed integer format, map range from -1.0 ; 1.0 to -32768 ; 32768.
% This is one of two sound formats accepted by OpenAL, the other being unsigned 8 bit
% integer in range 0;255. Other formats (e.g. float or double) are supported by some
% implementations, but one can't count on it. This is more efficient anyways...
mynoise = int16(mynoise * 32767);

% Fill our sound buffer with the data from the sound vector. Tell AL that its
% a 16 bpc, stereo format, with length(mynoise)*2*2 bytes total, to be played at
% a sampling rate of freq Hz. The AL will resample this to the native device
% sampling rate and format at buffer load time.
% We use stereo even for mono sounds, because stereo sounds are not
% spatialized by OpenAL, i.e., no 3D computations --> faster.
alBufferData( buffers, AL.FORMAT_STEREO16, mynoise, length(mynoise)*2*2, freq);

% Create a sound source:
source = alGenSources(1);

% Attach our buffer to it: The source will play the buffers sound data.
alSourceQueueBuffers(source, 1, buffers);

% Switch source to single shot.
alSourcei(source, AL.LOOPING, AL.FALSE);

% Set emission volume to 100%, aka a gain of 1.0:
alSourcef(source, AL.GAIN, 1);

% Set source to head-relative: In some implementations this skips some
% source<->listener 3D transformations. Our source sits in the origin
% (0,0,0) by default, as does our listener. --> No 3D spatialization.
alSourcei(source, AL.SOURCE_RELATIVE, AL.TRUE);

alSourcePlay(source);
WaitSecs(0.5);
offset = alGetSourcef(source, AL.SAMPLE_OFFSET);
alSourceStop(source);

% Perform some preflip to heat up the pipe:
screenid = max(Screen('Screens'));
win = Screen('OpenWindow', screenid, 0);

% Wait for keypress.
while KbCheck; end;
KbWait;

% Realtime scheduling:
% Priority(MaxPriority(win));

for i=1:10

Screen('Flip', win);

% Prepare black white transition:
Screen('FillRect', win, 255);
Screen('DrawingFinished', win);

% Wait a few seconds...
WaitSecs(0.66);

% Ok, the next flip will do a black-white transition...
[vbl visual_onset t1] = Screen('Flip', win);

% Start playback for this source:
alSourcePlay(source);
t2 = GetSecs;
tstart = t2;
%WaitSecs(0.020);

% Spin-Wait until hw reports the first sample is played...
offset = 0;
while offset == 0
    offset = alGetSourcef(source, AL.SAMPLE_OFFSET);
    t3=GetSecs;
end

fprintf('Expected visual onset at %6.6f\n', visual_onset);
fprintf('Sound started between %6.6f and  %6.6f\n', t1, t2);
fprintf('Expected latency sound - visual = %6.6f\n', t2 - visual_onset);
fprintf('First sound buffer played at %6.6f\n', t3);
fprintf('Delay start vs. played: %6.6f secs, offset %f\n', t3 - t2, offset);

WaitSecs(0.2);

% Stop playback:
alSourceStop(source);

Screen('FillRect', win, 0);
telapsed = Screen('Flip', win) - visual_onset

end

Priority(0);

% Unqueue sound buffer:
alSourceUnqueueBuffers(source, 1);

% Wait a bit:
WaitSecs(0.1);

% Delete buffer:
alDeleteBuffers(1, buffers);

% Wait a bit:
WaitSecs(0.1);

% Delete source:
alDeleteSources(1, source);

% Wait a bit:
WaitSecs(0.1);

% Shutdown OpenAL:
CloseOpenAL;
Screen('CloseAll');

% Done. Bye.
return;
