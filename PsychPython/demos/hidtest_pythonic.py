# hidtest.py - Basic test of PsychHID for Python
# Also tests WaitSecs, as it is utilized here.
#
# Collects key responses for 5 seconds via KbQueues while being blocked on
# the main thread. The disabled latter part will perform some low-level access
# to a USB-HID device, to test HID report send/receive. However, this is so
# device specific as being useless without customization to your specific machine
# OS and HID device. Rest assured it has some use for Mario's laptop.
#
# The test will be subjec to change without notice, it is throwaway wip code.
#
# (c) 2018 Mario Kleiner - Licensed under MIT license.

from psychtoolbox import hid
import psychtoolbox as ptb

# print("Devices:", hid.devices())
for dev in hid.devices():
    print(dev['usagePageValue'], dev['usageValue'], dev['usageName'], dev['product'])

kbInds, kbNames, kbDevices = hid.get_keyboard_devices()
print(kbInds, kbNames)

def run():

    # KbQueueTest
    ptb.WaitSecs('YieldSecs', 1)
    keys = [1] * 256
    keyboard = hid.Keyboard(kbInds[0])

    # keyboard.queue_create(num_slots=10000)  # not really needed

    # keyboard.start_trapping()
    # ptb.PsychHID('Keyboardhelper', -12)  # stops keys going to stdout

    keyboard.queue_start()
    ptb.WaitSecs('YieldSecs', 5)
    keyboard.queue_stop()

    # keyboard.stop_trapping()
    # ptb.PsychHID('Keyboardhelper', -10)  # restarts keys going to stdout

    while keyboard.flush():
        evt = keyboard.queue_get_event()
        print(evt)


# if this was run as a script (not an import) then call run()
if __name__ == "__main__":
    run()
