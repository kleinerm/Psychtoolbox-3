# ppatest.py - Basic test of PsychPortAudio for Python
# Also tests WaitSecs, GetSecs, and PsychHID basics, as
# they are utilized here.
#
# Plays a beep tone, then another beep tone, then records sound for 10 seconds,
# then plays it back until the end, or until a key is pressed.
# The test will be subjec to change without notice, it is throwaway test code.
#
# (c) 2019 Jon Peirce - Licensed under MIT license.
# (c) 2018 Mario Kleiner - Licensed under MIT license.

from psychtoolbox import audio, hid
from psychtoolbox import GetSecs, WaitSecs  # not yet "Pythonic"
from math import *
import numpy as np


# from soundfile import SoundFile

def printstatus(stream):
    info = stream.status
    print(info)
    return (info['Active'])


def run():
    # Samplerate:
    Fs = 48000

    # Tone frequency:
    f = 500

    ####################################################################
    ############## 1st test: play some tones ###########################
    ####################################################################

    audio.verbosity(5)

    # Open audio device: Default audio device, default opmode (playback), default
    # latency/timing-precision (low-latency, high precision), sample rate Fs,
    # stereo (2) channel output:
    stream = audio.Stream(freq=Fs, channels=2)
    stream.volume = 0.5  # PsychPortAudio('Volume', pahandle, 0.5)

    if True:
        # Length of audio vector - 1 secs of playback:
        n_samples = 1 * Fs
        # Build sinewave:
        a = np.sin(np.linspace(0, 2 * pi * f * n_samples, n_samples))
        # Replicated into two rows - One row for each stereo-channel, ie.
        # m'th row == m'th channel, n'th column = n'th sample frame:
        stereowav = np.array([a, a], order='f').transpose().astype('float32')
    else:
        # Make it less boring:
        fname = '/home/kleinerm/Music/test.wav'
        myfile = SoundFile(fname)
        stereowav = myfile.read(dtype='float32', always_2d=True)
        # stereowav = myfile.read()
        myfile.close()

    print('Type', type(stereowav), 'Shape', stereowav.shape, 'Datatype',
          stereowav.dtype, 'Order', stereowav.flags)

    t1 = GetSecs()
    stream.fill_buffer(stereowav)  # PsychPortAudio('FillBuffer', pahandle, stereowav);
    t2 = GetSecs()
    d1 = (1000 * (t2 - t1))
    print('FillBuffer Duration', d1, ' msecs.')

    # Start playback for one repetition (1), 1 seconds from now, wait for onset:
    stream.start(repetitions=1, when=GetSecs() + 1, wait_for_start=1)
    # PsychPortAudio('Start', pahandle, 1, GetSecs() + 1, 1)

    # Go into a loop that prints playback status once a second, while playback
    # is active:

    info = stream.status
    print(info, ' Spec ', info['Active'])
    while printstatus(stream):
        WaitSecs('YieldSecs', 1)  # didn't write a pythonic wrapper yet

    # Wait for sound to stop by itself, block until then:
    startTime, endPositionSecs, xruns, estStopTime = stream.stop()
    print('StartTime', startTime, 'secs. Stop time', estStopTime, 'secs.\n');

    # as before but 1.5*note_freq
    a = np.sin(np.linspace(0, 2 * pi * f*1.5 * n_samples, n_samples))
    # Replicated into two rows - One row for each stereo-channel, ie.
    # m'th row == m'th channel, n'th column = n'th sample frame:
    stereowav2 = np.array([a, a], order='f').transpose().astype('float32')

    t1 = GetSecs()

    b1 = audio.Buffer(stream=stream, data=stereowav2)  # PsychPortAudio('CreateBuffer', pahandle, stereowav2);
    t2 = GetSecs()
    d2 = (1000 * (t2 - t1))
    print('CreateBuffer Duration', d2, ' msecs.')
    t1 = GetSecs()
    b1.fill_buffer(stereowav2)  # PsychPortAudio('FillBuffer', pahandle, b1)
    t2 = GetSecs()
    print('FillBuffer Duration', (1000 * (t2 - t1)), ' msecs.')
    print('d2 / d1 = ', d2 / d1)

    # PsychPortAudio('Start', pahandle, 1, GetSecs() + 1, 1)
    stream.start(when=GetSecs()+1, wait_for_start=1)
    WaitSecs('YieldSecs', 2)
    # [startTime, endPositionSecs, xruns, estStopTime] = PsychPortAudio('Stop', pahandle, 1);
    startTime, endPositionSecs, xruns, estStopTime = stream.stop()
    print('StartTime', startTime, 'secs.xx Stop time', estStopTime, 'secs.\n');

    # Close sound device:
    stream.close()  # PsychPortAudio('Close', pahandle);
    #
    #
    # ####################################################################
    # ############## 2nd test: Audio capture:#############################
    # ####################################################################
    # recording = audio.Stream(mode=2, freq=Fs, channels=2);  # pahandle = PsychPortAudio('Open', [], 2, 0, Fs, 2);
    #
    # # Preallocate an internal audio recording  buffer with a capacity of 10 seconds:
    # recording.get_audio_data(10)  # PsychPortAudio('GetAudioData', pahandle, 10);
    #
    # # Start audio capture immediately and wait for the capture to start.
    # # We set the number of 'repetitions' to zero,
    # # i.e. record until recording is manually stopped.
    # recording.start(repetitions=0, stop_time=None)  # PsychPortAudio('Start', pahandle, 0, 0, 1);
    #
    # # fetch just under 10s from that buffer
    # # PsychPortAudio('GetAudioData', pahandle, None, 9.9, None);
    # audiodata, a, b, c = recording.get_audio_data(min_secs=9.9)
    # print('Type', type(audiodata), 'Shape', audiodata.shape, 'Datatype',
    #       audiodata.dtype);
    #
    # recording.close()  # PsychPortAudio('Close', pahandle);
    #
    # playback = audio.Stream(freq=Fs, channels=2);
    # playback.volume = 1.0  # PsychPortAudio('Volume', pahandle, 1);
    #
    # # Fill in audio matrix for playback:
    # playback.fill_buffer(audiodata)  # PsychPortAudio('FillBuffer', pahandle, audiodata);
    #
    # # Start playback for one repetition (1), 5 seconds from now, wait for sound onset:
    # playback.start(1, GetSecs() + 1, 1)  #  PsychPortAudio('Start', pahandle, 1, GetSecs() + 1, 1)
    #
    # # Go into a loop that prints playback status once a second, as long as playback
    # # is active:
    # keyboard = hid.Keyboard()
    # info = playback.status  # PsychPortAudio('GetStatus', pahandle);
    # print(info, ' Spec ', info['Active']);
    # while playback.status['Active'] and not keyboard.check()[0]:
    #     WaitSecs('YieldSecs', 1);
    #
    # # Stop playback:
    # # [startTime, endPositionSecs, xruns, estStopTime] = PsychPortAudio('Stop', pahandle)
    # startTime, endPositionSecs, xruns, estStopTime = playback.stop()
    # print('StartTime', startTime, 'secs. Stop time', estStopTime, 'secs.\n');
    #
    # # Close sound device:
    # playback.close()  # PsychPortAudio('Close', pahandle);

if __name__ == '__main__':
    run()
