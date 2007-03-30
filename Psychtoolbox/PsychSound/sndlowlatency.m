function sndlowlatency

% Establish key mapping: ESCape aborts, Space toggles between auto-
% movement of sound source or user mouse controlled movement:
KbName('UnifyKeynames');
space = KbName('space');
esc = KbName('ESCAPE');

dummy=GetSecs;
WaitSecs(0.1);

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
% mynoise = int16(mynoise * 32767);

Snd('Play', mynoise, freq, 16);
WaitSecs(0.5);

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
Snd('Play', mynoise, freq, 16);
t2 = GetSecs;
tstart = t2;
%WaitSecs(0.020);

fprintf('Expected visual onset at %6.6f\n', visual_onset);
fprintf('Sound started between %6.6f and  %6.6f\n', t1, t2);
fprintf('Expected latency sound - visual = %6.6f\n', t2 - visual_onset);

WaitSecs(0.2);

% Stop playback:
Snd('Quiet');

Screen('FillRect', win, 0);
telapsed = Screen('Flip', win) - visual_onset

end

Priority(0);

Screen('CloseAll');

% Done. Bye.
return;
