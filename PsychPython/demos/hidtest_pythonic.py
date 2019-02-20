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

print("Devices:")
for dev in hid.devices():
    print("  %(index)s: %(product)s "
          "usagePageValue=%(usagePageValue)s, usageValue=%(usageValue)s"
          % dev)

print("Keyboards:")
kbIDs, kbNames, kbDevices = hid.get_keyboard_indices()
for dev in kbDevices:
    print("  %(index)s: %(product)s" % dev)

def run():

    dev = kbDevices[0]
    # KbQueueTest
    ptb.WaitSecs('YieldSecs', 1)
    keys = [1] * 256
    keyboard = hid.Keyboard(dev['index'])

    # keyboard.queue_create(num_slots=10000)  # done implicitly but can redo

    # keyboard.start_trapping() # stops keys going to stdout but sketchy!

    keyboard.queue_start()
    t0 = ptb.GetSecs()
    while ptb.GetSecs() < t0+5:
        if keyboard.flush():
            evt = keyboard.queue_get_event()
            print(evt)
    keyboard.queue_stop()

    # keyboard.stop_trapping()  # restarts keys going to stdout


# if this was run as a script (not an import) then call run()
if __name__ == "__main__":
    run()
