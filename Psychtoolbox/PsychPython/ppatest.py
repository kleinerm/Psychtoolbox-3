# ppatest.py - Basic test of PsychPortAudio for Python
# Also tests WaitSecs, GetSecs, and PsychHID basics, as
# they are utilized here.
#
# Plays a beep tone, then another beep tone, then records sound for 10 seconds,
# then plays it back until the end or a key is pressed.
# The test will be subjec to change without notice, it is throwaway wip code.
#
# (c) 2018 Mario Kleiner - Licensed under MIT license.

from psychtoolboxclassic import *
from math import *
import numpy as np

def printstatus(pahandle):
    info = PsychPortAudio('GetStatus', pahandle);
    print(info);
    return(info['Active']);

def run():

    # Samplerate:
    Fs = 48000;

    # Tone frequency:
    f = 500;

    # Length of audio vector - 1 secs of playback:
    sample = 1 * Fs;

    # Build sinewave:
    a = [0] * sample;

    for n in range(sample):
        a[n] = sin(2 * pi * f * n / Fs);

    # Replicated into two rows - One row for each stereo-channel, ie.
    # m'th row == m'th channel, n'th column = n'th sample frame:
    stereowav = [a, a];

    PsychPortAudio('Verbosity', 5);

    # Open audio device: Default audio device, default opmode (playback), default
    # latency/timing-precision (low-latency, high precision), sample rate Fs,
    # stereo (2) channel output:
    pahandle = PsychPortAudio('Open', [], [], [], Fs, 2);

    PsychPortAudio('Volume', pahandle, 0.5);

    # Fill in audio matrix for playback:
    stereowav = np.array(stereowav);
    #stereowav = np.transpose(stereowav);

    t1 = GetSecs();
    PsychPortAudio('FillBuffer', pahandle, stereowav);
    t2 = GetSecs();
    print('Duration', (1000 * (t2-t1)), ' msecs.');

    # Start playback for one repetition (1), 5 seconds from now, wait for sound onset:
    PsychPortAudio('Start', pahandle, 1, GetSecs() + 1, 1)

    # Go into a loop that prints playback status once a second, as long as playback
    # is active:
    info = PsychPortAudio('GetStatus', pahandle);
    print(info, ' Spec ', info['Active']);
    while printstatus(pahandle):
        WaitSecs('YieldSecs', 1);

    # Wait for sound to stop by itself, block until then:
    [startTime, endPositionSecs, xruns, estStopTime] = PsychPortAudio('Stop', pahandle, 1)
    print('StartTime', startTime, 'secs. Stop time', estStopTime, 'secs.\n');

    for n in range(sample):
        a[n] = sin(2 * pi * f * 1.5 * n / Fs);

    # Replicated into two rows - One row for each stereo-channel, ie.
    # m'th row == m'th channel, n'th column = n'th sample frame:
    stereowav2 = [a, a];

    stereowav2 = np.array(stereowav2);
    #stereowav2 = np.transpose(stereowav2);

    #b1 = PsychPortAudio('CreateBuffer', pahandle, stereowav2);
    #PsychPortAudio('FillBuffer', pahandle, b1);
    PsychPortAudio('FillBuffer', pahandle, stereowav2);
    PsychPortAudio('Start', pahandle, 1, GetSecs() + 1, 1)
    [startTime, endPositionSecs, xruns, estStopTime] = PsychPortAudio('Stop', pahandle, 1)
    print('StartTime', startTime, 'secs. Stop time', estStopTime, 'secs.\n');

    # Close sound device:
    PsychPortAudio('Close', pahandle);

    pahandle = PsychPortAudio('Open', [], 2, 0, Fs, 2);

    # Preallocate an internal audio recording  buffer with a capacity of 10 seconds:
    PsychPortAudio('GetAudioData', pahandle, 10);

    # Start audio capture immediately and wait for the capture to start.
    # We set the number of 'repetitions' to zero,
    # i.e. record until recording is manually stopped.
    PsychPortAudio('Start', pahandle, 0, 0, 1);

    [audiodata, a, b, c] = PsychPortAudio('GetAudioData', pahandle, None, 9.9, None, 0);
    print('Type', type(audiodata), 'Shape', audiodata.shape, 'Datatype', audiodata.dtype);

    PsychPortAudio('Close', pahandle);

    pahandle = PsychPortAudio('Open', [], [], [], Fs, 2);

    PsychPortAudio('Volume', pahandle, 1);

    # Fill in audio matrix for playback:
    PsychPortAudio('FillBuffer', pahandle, audiodata);
    #PsychPortAudio('FillBuffer', pahandle, np.transpose(audiodata));

    # Start playback for one repetition (1), 5 seconds from now, wait for sound onset:
    PsychPortAudio('Start', pahandle, 1, GetSecs() + 1, 1)

    # Go into a loop that prints playback status once a second, as long as playback
    # is active:
    info = PsychPortAudio('GetStatus', pahandle);
    print(info, ' Spec ', info['Active']);
    while printstatus(pahandle) and not PsychHID('KbCheck')[0]:
        WaitSecs('YieldSecs', 1);

    # Stop playback:
    [startTime, endPositionSecs, xruns, estStopTime] = PsychPortAudio('Stop', pahandle)
    print('StartTime', startTime, 'secs. Stop time', estStopTime, 'secs.\n');

    # Close sound device:
    PsychPortAudio('Close', pahandle);
