/*------------------------------------------------------------------------------
  RPiGPIOMex.c -- A simple MEX file for GPIO control on the RaspberryPi

  On Octave, compile with:

  mex -v -g RPiGPIOMex.c -lwiringPi

  ------------------------------------------------------------------------------

  Copyright (C) 2016 Mario Kleiner

  This program is licensed under the MIT license.

  A copy of the license can be found in the License.txt file inside the
  Psychtoolbox-3 top level folder.
------------------------------------------------------------------------------*/

/* Octave includes: */
#include "mex.h"

/* System includes */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/* wiringPi library for RPi GPIO control includes */
#include <wiringPi.h>

static bool firstTime = 1;
static bool sysMode = 1;

void exitfunc(void)
{
    // Actually nothing to do. The library does not have a shutdown function.
    firstTime = 1;
}

/* This is the main entry point from Octave: */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    int cmd, pin, arg;
    int rc;
    double* out;

    // Get our name for output:
    const char* me = mexFunctionName();

    // Special case: Called with one return argument and no input arguments. Return RPi board revision number:
    if (nrhs == 0 && nlhs == 1) {
        rc = piBoardRev();
        plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
        *(mxGetPr(plhs[0])) = (double) rc;
        return;
    }

    if (nrhs < 2) {
        mexPrintf("%s: A simple Octave MEX file for basic control of the RaspberryPi GPIO pins under GNU/Linux.\n\n", me);
        mexPrintf("(C) 2016 Mario Kleiner -- Licensed to you under the MIT license.\n");
        mexPrintf("This file is part of Psychtoolbox-3 but should also work independently.\n\n");
        mexPrintf("Pin numbers are in Broadcom numbering scheme aka BCM_GPIO numbering.\n");
        mexPrintf("Mapping to physical connector pins and other restrictions and pin properties can be found at\n");
        mexPrintf("http://wiringpi.com/pins \n\n");
        mexPrintf("For testing purposes, pins 35 and 47 on a RaspberryPi 2B map to the red power and green status LEDs.\n\n");
        mexPrintf("The gpio command line utility allows to setup and export pins for use by a non-root user.\n\n");
        mexPrintf("\n");
        mexPrintf("Usage:\n\n");
        mexPrintf("revision = %s;\n", me);
        mexPrintf("- Return RaspberryPi board 'revision' number. Different revisions == different pinout.\n\n");
        mexPrintf("state = %s(0, pin);\n", me);
        mexPrintf("- Query 'state' of pin number 'pin': 1 = High, 0 = Low.\n\n");
        mexPrintf("%s(1, pin, level);\n", me);
        mexPrintf("- Set state of pin number 'pin' to logic level 'level': 1 = High, 0 = Low.\n\n");
        mexPrintf("%s(2, pin, level);\n", me);
        mexPrintf("- Set pulse-width modulation state of pin number 'pin' to level 'level': 0 - 1023.\n");
        mexPrintf("  Only available on GPIO logical pin 18 (physical connector pin 12) on the RaspberryPi without extension boards.\n");
        mexPrintf("  Only available if running Octave as root, ie. started via sudo octave.\n\n");
        mexPrintf("%s(3, pin, direction);\n", me);
        mexPrintf("- Set direction of pin number 'pin' to 'direction'. 1 = Output, 0 = Input.\n");
        mexPrintf("  Only available if running Octave as root, ie. started via sudo octave.\n\n");
        mexPrintf("%s(4, pin, pullMode);\n", me);
        mexPrintf("- Set resistor mode of pin number 'pin' to 'pullMode'. -1 = Pull down, 1 = Pull up, 0 = None.\n");
        mexPrintf("  Pin must be configured as input for pullup/pulldown resistors to work.\n");
        mexPrintf("  Only available if running Octave as root, ie. started via sudo octave.\n\n");
        mexPrintf("result = %s(5, pin, timeoutMsecs);\n", me);
        mexPrintf("- Wait for rising/falling edge on input pin number 'pin' with a timeout of 'timeoutMsecs': -1 = Infinite wait.\n");
        mexPrintf("  Return 'result' status code: -1 = error, 0 = timed out, 1 = trigger received.\n");
        mexPrintf("  Pin must be configured as input and edge trigger type must be setup via the gpio utility.\n\n");

        return;
    }

    if (firstTime) {
        // Enable return of proper error codes if wiringPi setup fails.
        // We want to handle this gracefully instead of crashing:
        setenv("WIRINGPI_CODES", "1", 0);

        // Initialize wiringPi. If we are effectively root then we use
        // Gpio mode for full support, otherwise Sysmode for only basic
        // digital i/o.
        //
        // We use native Broadcom GPIO pin numbering:
        rc = wiringPiSetupSys();
        sysMode = 1;

        if (!geteuid()) {
            // Upgrade to root access mode:
            rc = wiringPiSetupGpio();
            sysMode = 0;
        }

        if (rc) {
            mexPrintf("wiringPi init failed: Error code %i [%s]\n", rc, strerror(rc));
            mexErrMsgTxt("Failed to initialize GPIO system.");
        }

        // Successfully connected. Register exit handler to close GPIO control
        // when mex file is flushed:
        mexAtExit(exitfunc);

        // Ready to rock:
        firstTime = 0;
    }

    // First argument must be the command code:
    cmd = (int) mxGetScalar(prhs[0]);

    // Second argument is pin number:
    pin = (int) mxGetScalar(prhs[1]);

    if (nrhs > 2)
        arg = (int) mxGetScalar(prhs[2]);
    else
        arg = -1000;

    switch (cmd) {
        case 0: // Read input level from pin: 1 = High, 0 = Low
            rc = digitalRead(pin);
            plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
            *(mxGetPr(plhs[0])) = (double) rc;
            break;

        case 1: // Write a new level to output pin: 1 = High, 0 = Low
            if (arg < 0)
                mexErrMsgTxt("New logic level of output pin missing for output pin write.");

            digitalWrite(pin, arg);
            break;

        case 2: // NOT in sysmode: Write a new pwm level to output pin: 0 to 1024 on RPi
            if (arg < 0)
                mexErrMsgTxt("New pwm level of output pin missing for output pin pulse-width modulation.");

            if (sysMode)
                mexErrMsgTxt("PWM control unsupported in sys mode! Must run as root via sudo to use this!");

            pwmWrite(pin, arg);
            break;

        case 3: // NOT in sysmode: Set pin mode: 1 = out, 0 = in
            if (arg < 0)
                mexErrMsgTxt("New opmode for pin missing for pin mode configuration.");

            if (sysMode)
                mexErrMsgTxt("PWM control unsupported in sys mode! Must run as root via sudo to use this!");

            pinMode(pin, arg ? OUTPUT : INPUT);
            break;

        case 4: // NOT in sysmode: Set pullup/pulldowns: 1 = out, 0 = in
            if (arg < -1)
                mexErrMsgTxt("New pullup/down for pin missing for pin resistor configuration.");

            if (sysMode)
                mexErrMsgTxt("PWM control unsupported in sys mode! Must run as root via sudo to use this!");

            pullUpDnControl(pin, (arg == 0) ? PUD_OFF : ((arg > 0) ? PUD_UP : PUD_DOWN));
            break;

        case 5: // Wait for rising or falling edge on given input pin via interrupt.
            if (arg < -1)
                mexErrMsgTxt("Timeout value in milliseconds missing.");

            rc = waitForInterrupt(pin, arg);
            plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
            *(mxGetPr(plhs[0])) = (double) rc;
            break;

        default:
            mexErrMsgTxt("Unknown command code provided!");
    }

    // Done.
    return;
}
