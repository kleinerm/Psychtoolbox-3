"""

PsychHID('KbQueueCreate', [deviceNumber][, keyFlags=all][, numValuators=0][, numSlots=10000][, flags=0][, windowHandle=0])
PsychHID('KbQueueRelease' [, deviceIndex])
[navail] = PsychHID('KbQueueFlush' [, deviceIndex][, flushType=1])
PsychHID('KbQueueStart' [, deviceIndex])
PsychHID('KbQueueStop' [, deviceIndex])
[keyIsDown, firstKeyPressTimes, firstKeyReleaseTimes, lastKeyPressTimes, lastKeyReleaseTimes]=PsychHID('KbQueueCheck' [, deviceIndex])
secs=PsychHID('KbTriggerWait', KeysUsage, [deviceNumber])
[event, navail] = PsychHID('KbQueueGetEvent' [, deviceIndex][, maxWaitTimeSecs=0])


Support for access to generic USB devices: See 'help ColorCal2' for one usage example:


usbHandle = PsychHID('OpenUSBDevice', vendorID, deviceID [, configurationId=0])
PsychHID('CloseUSBDevice' [, usbHandle])
outData = PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength, inData)

"""
from . import PsychHID


def num_devices():
    return PsychHID('NumDevices')


def devices(device_class=None):
    return PsychHID('Devices', device_class)


class Device:
    def __init__(self, device_number):
        self.device_number = device_number

    def num_elements(self):
        return PsychHID('NumElements', self.device_number)

    def elements(self):
        return PsychHID('Elements', self.device_number)

    def num_collections(self):
        return PsychHID('NumCollections', self.device_number)

    def collections(self):
        return PsychHID('Collections', self.device_number)

    def get_state(self, element_number, calibrated=False):
        if calibrated:
            return PsychHID('CalibratedState', self.device_number,
                            element_number)
        else:
            return PsychHID('RawState', self.device_number, element_number)

    def get_report(self, report_type, report_id, report_bytes):
        """Returns a report and error code for the device

        :param report_type:
        :param report_id:
        :param report_bytes:
        :return: [report, err]
        """
        return PsychHID('GetReport', self.device_number)

    def set_report(self, report_type, report_id, report):
        """"""
        return PsychHID('SetReport', self.device_number,
                        report_type, report_id, report)

    def give_me_reports(self, report_bytes=None):
        """:returns: [reports, err]"""
        return PsychHID('GiveMeReports', self.device_number, report_bytes)

    def receive_reports(self, options=None):
        """:returns: err code"""
        return PsychHID('ReceiveReports', self.device_number, options)

    def stop_reports(self):
        """:returns: err code"""
        return PsychHID('ReceiveReportsStop', self.device_number)


class Keyboard():
    def __init__(self, device_number=None):
        self.device_number = device_number

    def check(self, scan_list=None):
        return PsychHID('KbCheck', self.device_id, scan_list)

    def _create_queue(self, num_slots=10000, flags=0, win_handle=0):
        PsychHID('KbQueueCreate', self.device_number,
                 None, 0, num_slots, flags, win_handle)
        # [deviceNumber][, keyFlags=all][, numValuators=0][, numSlots=10000]
        # [, flags=0][, windowHandle=0])

    def _release_queue(self):
        PsychHID('KbQueueRelease', self.device_number)

    def flush(self, flush_type=1):
        """Flushes the keybard queue and returns the number of available evts"""
        return PsychHID('KbQueueFlush', self.device_number, flush_type)

    def queue_start(self):
        """Starts recording key presses to the queue"""
        PsychHID('KbQueueStart', self.device_number)

    def queue_stop(self):
        """Stops recording key presses to the queue"""
        PsychHID('KbQueueStop', self.device_number)

    def queue_check(self):
        """Returns key events from the buffer
        :param self:
        :return: [keyIsDown, firstKeyPressTimes, firstKeyReleaseTimes, lastKeyPressTimes,
         lastKeyReleaseTimes]
        """
        return PsychHID('KbQueueCheck', self.device_number)

    def queue_get_event(self, max_wait_secs=0):
        """

        :param max_wait_secs:
        :return: [event, navail]
        """
        return PsychHID('KbQueueGetEvent', self.device_number, max_wait_secs)

    def trigger_wait(self, keys):
        secs=PsychHID('KbTriggerWait', keys, self.device_number)
