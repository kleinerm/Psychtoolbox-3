"""Converts Psychtoolbox formatted functions into prettier python-styling

RIGHT NOW THIS IS A WORK IN PROGRESS. ALTHOUGH THE PsychPortAudio LIB HAS BEEN
TESTED BY MARIO, this additional wrapper is under development. Use with extreme
caution only for development purposes right now.


e.g.::

    from psychtooolbox import PsychPortAudio
    pahandle = PsychPortAudio('Open', [], [], [0], Fs, 2)
    PsychPortAudio('FillBuffer', pahandle, stereowav)
    PsychPortAudio('Start', pahandle)

becomes::

    from psychtooolbox import portaudio
    stream = portaudio.Stream([], [], [0], Fs, 2)
    stream.fill_buffer(stereowav)
    stream.start()

(c) 2018 Jon Peirce - Licensed under MIT license.
"""

"""
Function summary from PsychPortAudio for not-yet-implemented functions:

pahandle = PsychPortAudio('OpenSlave', pamaster [, mode][, channels]
    [, selectchannels]);
    
PsychPortAudio('DeleteBuffer'[, bufferhandle] [, waitmode]);
PsychPortAudio('RefillBuffer', pahandle [, bufferhandle=0], bufferdata
    [, startIndex=0]);
PsychPortAudio('SetLoop', pahandle[, startSample=0][, endSample=max]
    [, UnitIsSeconds=0]);

PsychPortAudio('UseSchedule', pahandle, enableSchedule [, maxSize = 128]);
[success, freeslots] = PsychPortAudio('AddToSchedule', pahandle
    [, bufferHandle=0][, repetitions=1][, startSample=0][, endSample=max]
    [, UnitIsSeconds=0][, specialFlags=0]);
startTime = PsychPortAudio('RescheduleStart', pahandle, when [, waitForStart=0]
    [, repetitions] [, stopTime]);
"""
import atexit
from . import PsychPortAudio

def get_version_info():
    return PsychPortAudio('Version')


def verbosity(level=None):
    """Gets or sets the verbosity level (integer 1-5).

    If no level is provided the current level of verbosity is returned.
    If a new level is provided the old level setting is returned"""
    oldlevel = PsychPortAudio('Verbosity', level)


def get_open_device_count():
    return PsychPortAudio('GetOpenDeviceCount')


def get_devices(device_type=None, device_index=None):
    """Get a list of devices on the current system, optionally of certain types

    :param device_type:
    :param device_index:
    :return:
    """
    return PsychPortAudio('GetDevices', device_type, device_index);


def tune_engine(yield_interval, mutex_enable, lock_to_core1,
                audioserver_autosuspend):
    """Sets values for various advanced tuning parameters for the audio engine

    :param yield_interval:
    :param mutex_enable:
    :param lock_to_core1:
    :param audioserver_autosuspend:
    :return: the 4 values in their final state
    """
    return PsychPortAudio('EngineTunables', yield_interval, mutex_enable,
                          lock_to_core1, audioserver_autosuspend)


class Stream():
    def __init__(self, deviceid, mode, reqlatencyclass, freq, channels,
                 buffersize=None, suggested_latency=None, selectchannels=None,
                 flags=0):
        self.handle = PsychPortAudio('Open', deviceid, mode,
                                     reqlatencyclass,
                                     freq, channels, buffersize,
                                     suggested_latency, selectchannels,
                                     flags)
        atexit.register(self.close)

    def start(self, repetitions=1, when=0, wait_for_start=0,
              stop_time=None, resume=0):
        """Start the stream for a given number of repetitions, potentially with
        a delayed start.

        :param repetitions:
        :param when:
        :param wait_for_start:
        :param stop_time:
        :param resume:
        :return:
        """
        start_time = PsychPortAudio('Start', self.handle, repetitions, when,
                                   wait_for_start, stop_time, resume)
        return start_time

    def stop(self, wait_for_end_playback=0, block_until_stopped=1,
             repetitions=None, stopTime=None):
        """
        :param wait_for_end_playback:
        :param block_until_stopped:
        :param repetitions:
        :param stopTime:
        :return: startTime, endPositionSecs, xruns, estStopTime
        """
        return PsychPortAudio('Stop', self.handle,
                              wait_for_end_playback, block_until_stopped,
                              repetitions, stopTime)

    def close(self):
        """Close the current stream"""
        try:
            PsychPortAudio('Close', self.handle)
        except Exception as err:
            if "Invalid audio device handle" in str(err):
                # this most likely means the stream is closed already
                return
            else:
                raise err

    def setVolume(self, masterVolume=None, channelVolumes=None):
        """As well as being able to use the volume_master and volume_channels
        attributes, you can use this function to set both master and channel
        volumes at the same time"""
        PsychPortAudio('Volume', self.handle, masterVolume, channelVolumes)

    @property
    def status(self):
        """The status of this portaudio stream"""
        return PsychPortAudio('GetStatus', self.handle)

    @property
    def volume(self):
        """A property allowing you to get/set the stream's master volume

        see also:
            volume_channels
        """
        return PsychPortAudio('Volume', self.handle)[0]

    @volume.setter
    def volume(self, volume):
        PsychPortAudio('Volume', self.handle, volume, None)

    @property
    def volume_channels(self):
        """A property allowing you to get/set the volume independently in each
        channel

        see also:
            volume_master
        """
        return PsychPortAudio('Volume', self.handle)[1]

    @volume_channels.setter
    def volume_channels(self, volumes):
        PsychPortAudio('Volume', self.handle, None, volumes)

    @property
    def run_mode(self):
        """A property to get or set the stream's current run_mode"""
        return PsychPortAudio('RunMode', self.handle)

    @run_mode.setter
    def run_mode(self, mode):
        PsychPortAudio('RunMode', self.handle, mode)

    @property
    def latency_bias(self):
        """A property to get/set the stream's latency bias"""
        return PsychPortAudio('LatencyBias', self.handle)

    @latency_bias.setter
    def latency_bias(self, secs):
        PsychPortAudio('LatencyBias', self.handle, secs)

    @property
    def op_mode(self):
        """Equivalent to PsychPortAudio('SetOpMode', op_mode)"""
        return PsychPortAudio('SetOpMode', self.handle)

    @op_mode.setter
    def op_mode(self, op_mode):
        PsychPortAudio('SetOpMode', self.handle, op_mode)

    def get_audio_data(self, secs_allocate=None,
                       min_secs=None, max_secs=None, single_type=1):
        """Get audio data from the port audio stream (e.g. from the mic)
        
        :param secs_allocate: 
        :param min_secs: minimum seconds returned
        :param max_secs: maximum seconds returned
        :param single_type: 
        :return: (audiodata, absrecposition, overflow, cstarttime)
        """
        return PsychPortAudio('GetAudioData', self.handle,
                              secs_allocate, min_secs, max_secs,
                              single_type)

    def fill_buffer(self, data):
        """Create a new buffer and fill with audio data to be played when
        stream starts"""
        buffer = Buffer(stream=self, data=data)
        return buffer

    def __del__(self):
        self.close()


class Buffer():
    def __init__(self, stream, data=None):
        self.stream = stream
        if data is None:
            self.handle = PsychPortAudio('CreateBuffer', self.stream.handle,
                                         data)
        else:
            self.handle = PsychPortAudio('FillBuffer', self.stream.handle,
                                         data)

    def fill_buffer(self, data, streaming=0, startIndex='append'):
        return PsychPortAudio('FillBuffer', self.stream.handle,
                              data, streaming, startIndex)
