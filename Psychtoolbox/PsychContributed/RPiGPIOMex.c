/*------------------------------------------------------------------------------
  RPiGPIOMex.c -- A simple MEX file for GPIO control on the RaspberryPi

  On Octave, compile with:

  mex -v -g RPiGPIOMex.c -lwiringPi

  ------------------------------------------------------------------------------

  Copyright (C) 2016 - 2023 Mario Kleiner

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

void exitfunc(void)
{
    // Actually nothing to do. The library does not have a shutdown function.
    firstTime = 1;
}

unsigned long long nowMS(void)
{
    static unsigned int thigh = 0;
    static unsigned int oldT = 0;
    unsigned int tlow = millis();

    // 32-Bit time wraparound? Increase upper 32-Bit:
    if (tlow < oldT)
        thigh ++;

    // Keep track of current lower 32-Bits for wraparound handling:
    oldT = tlow;

    // Assemble new 64-Bit time:
    return ((unsigned long long) thigh) << 32 | ((unsigned long long) tlow);
}

/* waitFor 'pin' reaching target level 'waitLevel', or 'timeout' msecs elapsed. */
int waitFor(int pin, int timeout, int waitLevel)
{
    // Compute deadline for timeout - -1 == Infinite wait:
    unsigned long long timeoutD = (timeout >= 0) ? (nowMS() + (unsigned long long) timeout) : 0xffffffffffffffff;

    // Wait for signal level not being waitLevel:
    while ((nowMS() < timeoutD) && (digitalRead(pin) == waitLevel))
        delayMicroseconds(200);

    // Timed out?
    if (digitalRead(pin) == waitLevel)
        return 0;

    // Wait for signal level transition to waitLevel:
    while ((nowMS() < timeoutD) && (digitalRead(pin) != waitLevel))
        delayMicroseconds(200);

    // Timed out?
    if (digitalRead(pin) != waitLevel)
        return 0;

    // Success:
    return 1;
}

/* This is the main entry point from Octave: */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    int cmd, pin, arg, waitRising;
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
        mexPrintf("(C) 2016 - 2023 Mario Kleiner -- Licensed to you under the MIT license.\n");
        mexPrintf("This file is part of Psychtoolbox-3 but should also work independently.\n\n");
        mexPrintf("Pin numbers are in Broadcom numbering scheme aka BCM_GPIO numbering.\n");
        mexPrintf("Mapping to physical connector pins and other restrictions and pin properties can be found at\n");
        mexPrintf("http://wiringpi.com/pins or by typing the command 'pinout' into a terminal.\n\n");
        mexPrintf("For testing purposes, pins 35 and 47 on a RaspberryPi 2B map to the red power and green status LEDs.\n\n");
        mexPrintf("The gpio command line utility allows to setup and export pins from a shell. Octave must be run as\n");
        mexPrintf("root user via 'sudo octave' or the user must be part of the 'gpio' Unix user group for non-root operation.\n\n");
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
        mexPrintf("  Only available on GPIO logical pin 18 (physical connector pin 12) on the RaspberryPi without extension boards.\n\n");
        mexPrintf("%s(3, pin, direction);\n", me);
        mexPrintf("- Set direction of pin number 'pin' to 'direction'. 1 = Output, 0 = Input.\n\n");
        mexPrintf("%s(4, pin, pullMode);\n", me);
        mexPrintf("- Set resistor mode of pin number 'pin' to 'pullMode'. -1 = Pull down, 1 = Pull up, 0 = None.\n");
        mexPrintf("  Pin must be configured as input for pullup/pulldown resistors to work.\n\n");
        mexPrintf("result = %s(5, pin, timeoutMsecs);\n", me);
        mexPrintf("- Wait for rising edge on input pin number 'pin' with a timeout of 'timeoutMsecs': -1 = Infinite wait.\n");
        mexPrintf("  Return 'result' status code: -1 = error, 0 = timed out, 1 = trigger received.\n");
        mexPrintf("  Pin must be configured as input.\n\n");

        return;
    }

    if (firstTime) {
        // Enable return of proper error codes if wiringPi setup fails.
        // We want to handle this gracefully instead of crashing:
        setenv("WIRINGPI_CODES", "1", 0);

        // Always use GPIO mode - requires sudo root, or user to be in 'gpio' Unix user group:
        rc = wiringPiSetupGpio();
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

            pwmWrite(pin, arg);
            break;

        case 3: // NOT in sysmode: Set pin mode: 1 = out, 0 = in
            if (arg < 0)
                mexErrMsgTxt("New opmode for pin missing for pin mode configuration.");

            pinMode(pin, arg ? OUTPUT : INPUT);
            break;

        case 4: // NOT in sysmode: Set pullup/pulldowns: 1 = out, 0 = in
            if (arg < -1)
                mexErrMsgTxt("New pullup/down for pin missing for pin resistor configuration.");

            pullUpDnControl(pin, (arg == 0) ? PUD_OFF : ((arg > 0) ? PUD_UP : PUD_DOWN));
            break;

        case 5: // Wait for rising or falling edge on given input pin via polling.
            if (arg < -1)
                mexErrMsgTxt("Timeout value in milliseconds missing.");

            // Wait for rising edge:
            waitRising = 1;

            // Wait for target level with timeout:
            rc = waitFor(pin, arg, waitRising);
            plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
            *(mxGetPr(plhs[0])) = (double) rc;
            break;

        default:
            mexErrMsgTxt("Unknown command code provided!");
    }

    // Done.
    return;
}
