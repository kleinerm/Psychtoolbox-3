/*
 * openhmdkeepalivedaemon - Keep specific HMD's alive (powered up and discoverable
 * by the display system) so they are more easy to handle for software that has
 * trouble dynamically discovering their associated video outputs if the HMD has
 * its display powered down, e.g., X-Servers in multi-x-screen configuration, or
 * Psychtoolbox Screen() function. The list of HMD which require this treatment
 * is hard-coded.
 *
 * This daemon checks for attached but unused OpenHMD supported HMD's like the
 * CV1 every 5 seconds. If it finds such a device, it opens and closes the device,
 * thereby powering up the device and its display, and (re)arming the devices keep
 * alive timer to keep the device awake for a couple of seconds. The cycle repeats
 * often enough to keep such devices permanently active.
 *
 * Requires libopenhmd.so (http://www.openhmd.net) and its dependencies.
 *
 * Compile with: gcc -o openhmdkeepalivedaemon openhmdkeepalivedaemon.c -lc -lopenhmd -lm
 *
 * Run as daemon without runtime debug output: openhmdkeepalivedaemon
 * Run in session with verbose runtime debug: openhmdkeepalivedaemon -v
 *
 * Copyright (c) 2017 Mario Kleiner.
 *
 * Licensed to you under the MIT license:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "openhmd.h"

struct ohmd_context *ctx;
int verbose;
int byebye = 0;

void doterminate(int sig)
{
    (void) sig;
    byebye = 1;
}

int main(int argc, char* argv[])
{
    ohmd_device* hmd;
    int available_devices;
    int deviceIndex;
    verbose = ((argc > 1) && !strcmp(argv[1], "-v")) ? 1 : 0;

    if ((ctx = ohmd_ctx_create()) == NULL) {
        printf("%s: Initialization of OpenHMD runtime failed. Exiting...\n", argv[0]);
        return(-1);
    }

    signal(SIGTERM, &doterminate);
    signal(SIGINT, &doterminate);

    printf("%s: OpenHMD VR runtime initialized, getting to work.", argv[0]);

    if (verbose)
        printf("\n");
    else
        printf(" Turning into a background daemon: 'killall %s' to terminate me.\n", argv[0]);

    fflush(NULL);

    // If no debug output at runtime is requested, turn us into a background daemon:
    if (!verbose)
        if (daemon(1, 0))
            perror("Failed to daemonize!");

    // Infinite probe and keepalive loop:
    while (!byebye) {
        const int noautoupdate = 0;

        // Get count of available devices:
        available_devices = ohmd_ctx_probe(ctx);

        // Iterate over all devices:
        for (deviceIndex = 0; deviceIndex < available_devices; deviceIndex++) {
            // Skip HMDs which don't need this kind of treatment:
            if ((!strstr(ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT), "Rift (CV")) &&
                (!strstr(ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT), "Deepoon")) &&
                (!strstr(ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT), "PSVR")) &&
                (!strstr(ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT), "Vive")))
                continue;

            // Disable auto-updater thread for this one-shot init & keep alive:
            ohmd_device_settings *settings = ohmd_device_settings_create(ctx);
            ohmd_device_settings_seti(settings, OHMD_IDS_AUTOMATIC_UPDATE, &noautoupdate);

            // Try to open it. If this works, this means the HMD is not in use
            // by actual VR apps, so we keep it alive by opening it.
            hmd = ohmd_list_open_device_s(ctx, deviceIndex, settings);
            ohmd_device_settings_destroy(settings);

            if (hmd) {
                // Opening it powered its display up if it was off, and set a keep alive
                // timeout of at least a couple seconds (10 secs i think?), so it will
                // stay up for at least 10 seconds after we close it. Now we can close
                // it, so actual VR apps can use it, at least for non Vive devices:
                if (!strstr(ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT), "Vive")) {
                    ohmd_close_device(hmd);
                }
                else {
                    // HTC Vive is special: OpenHMD would power down the Vive and its
                    // display on ohmd_close_device(), so we can't close the device.
                    // However we must release the underlying HID device for actual VR
                    // renderers to access the Vive. We can't call ohmd_ctx_destroy()
                    // either, as that would imply ohmd_close_device.
                    // So we do something dirty: We terminate ourselves here without
                    // properly closing down anything. This will not power down the Vive,
                    // but the OS will release the HID device on process termination, so
                    // actual clients can access it. The downside of this is we will
                    // be gone after one cycle, so mixing Vives with other HMDs would
                    // not work well.
                    byebye = 2;
                }

                if (verbose)
                    printf("%s: Keep alive cycle done for HMD %i: %s [%s].\n", argv[0], deviceIndex,
                           ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT),
                           ohmd_list_gets(ctx, deviceIndex, OHMD_PATH));
            }
            else if (verbose)
                printf("%s: HMD %i: %s [%s] in use at the moment, no need to keep alive.\n", argv[0],
                       deviceIndex, ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT),
                       ohmd_list_gets(ctx, deviceIndex, OHMD_PATH));
        }

        // Sleep for 5 seconds before next keep alive cycle. Should be
        // short enough for no unattended HMD to time out:
        if (!byebye)
            sleep(5);

        // Rinse, wash, repeat.
    }

    // Skip  context destroy for special devices like the Vive (see above):
    if (byebye != 2)
        ohmd_ctx_destroy(ctx);

    if (verbose)
        printf("%s: Exiting OpenHMD keep alive daemon.\n", argv[0]);

    fflush(stdout);

    return(0);
}
