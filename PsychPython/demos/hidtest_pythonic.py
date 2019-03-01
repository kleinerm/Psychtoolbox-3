# hidtest.py - Basic test of PsychHID for Python
# Also tests WaitSecs, as it is utilized here.
#
# The test will be subject to change without notice, it is throwaway wip code.
#
# (c) 2019 Jon Peirce - Licensed under MIT license.
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
    keyboard = hid.Keyboard(dev['index'])

    # keyboard.queue_create(num_slots=10000)  # done implicitly but can redo

    # keyboard.start_trapping() # stops keys going to stdout but sketchy!

    keyboard.queue_start()
    t0 = ptb.GetSecs()
    while ptb.GetSecs() < t0+5:
        ptb.WaitSecs('YieldSecs', 0.00001)
        if keyboard.flush():
            evt = keyboard.queue_get_event()
            print(evt)
    keyboard.queue_stop()

    # keyboard.stop_trapping()  # restarts keys going to stdout


# if this was run as a script (not an import) then call run()
if __name__ == "__main__":
    run()
