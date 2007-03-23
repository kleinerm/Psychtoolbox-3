function palowlatency

% Establish key mapping: ESCape aborts, Space toggles between auto-
% movement of sound source or user mouse controlled movement:
KbName('UnifyKeynames');
space = KbName('space');
esc = KbName('ESCAPE');

dummy=GetSecs;
WaitSecs(0.1);

if nargin < 1
    deviceid = -1;
end

deviceid

reqlatencyclass = 1;
freq = [];
buffersize = [];

pahandle = PsychPortAudio('Open', deviceid, [], reqlatencyclass, freq, 2, buffersize);


% Start off with 10 seconds of freq hz random stereo noise as a fallback:
freq = 44100;
mynoise = randn(2,freq * 0.1);

% Fill buffer with data:
PsychPortAudio('FillBuffer', pahandle, mynoise);

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
audio_onset = PsychPortAudio('Start', pahandle, 0, 0, 0);
t2 = GetSecs;
tstart = t2;

% Spin-Wait until hw reports the first sample is played...
offset = 0;
while offset == 0
    status = PsychPortAudio('GetStatus', pahandle);
    offset = status.PositionSecs;
    t3=GetSecs;
end
audio_onset = status.StartTime;

%fprintf('Expected visual onset at %6.6f secs.\n', visual_onset);
%fprintf('Sound started between %6.6f and  %6.6f\n', t1, t2);
%fprintf('Expected latency sound - visual = %6.6f\n', t2 - visual_onset);
%fprintf('First sound buffer played at %6.6f\n', t3);
%fprintf('Delay start vs. played: %6.6f secs, offset %f\n', t3 - t2, offset);

fprintf('Screen    expects visual onset at %6.6f secs.\n', visual_onset);
fprintf('PortAudio expects audio onset  at %6.6f secs.\n', audio_onset);
fprintf('Expected audio-visual delay    is %6.6f msecs.\n', (audio_onset - visual_onset)*1000.0);
WaitSecs(0.2);

% Stop playback:
PsychPortAudio('Stop', pahandle);

Screen('FillRect', win, 0);
telapsed = Screen('Flip', win) - visual_onset;

end

Priority(0);

PsychPortAudio('Close');
Screen('CloseAll');

% Done. Bye.
return;
