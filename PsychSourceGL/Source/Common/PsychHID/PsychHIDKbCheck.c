/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDKbCheck.c

  PROJECTS: PsychHID only.

  PLATFORMS:  All.

  AUTHORS:

  Allen.Ingling@nyu.edu             awi
  mario.kleiner.de@gmail.com        mk

  HISTORY:
  5/12/03  awi      Created.
  12/17/09 rpw      Added keypad support
  07/28/11 mk       Refactored for multi-os support.

  TO DO:

*/

#include "PsychHID.h"

static char useString[] = "[keyIsDown,secs,keyCode]=PsychHID('KbCheck' [, deviceNumber][, scanList])";
static char synopsisString[] =
        "Scan a keyboard, keypad, or other HID device with buttons, and return a vector of logical values indicating the "
        "state of each key.\n"
        "By default the default keyboard device (as determined by some operating system dependent heuristic) is "
        "scanned. If no keyboard is found, the first keypad device is scanned, followed by other "
        "devices, e.g., mice. Optionally, the 'deviceNumber' of any keyboard or HID device may be specified.\n"
        "As checking all potentially 256 keys on a HID device is a time consuming process, "
        "which can easily take up to 1 msec on modern hardware, you can restrict the scan to a subset of "
        "the 256 keys by providing the optional 'scanList' parameter: 'scanList' must be a vector of 256 "
        "doubles, where the i'th element corresponds to the i'th key and a zero value means: Ignore this "
        "key during scan, whereas a positive non-zero value means: Scan this key.\n"
        "The PsychHID('KbCheck') implements the KbCheck command as provided by the  OS 9 Psychtoolbox. "
        "KbCheck is defined in Psychtoolbox-3 and invokes PsychHID('KbCheck'). "
        "Always use KbCheck instead of directly calling PsychHID('KbCheck'), unless you have very good "
        "reasons to do otherwise and really know what you're doing!";

static char seeAlsoString[] = "";

PsychError PSYCHHIDKbCheck(void)
{
    int deviceIndex;
    int m, n, p;
    double *scanList = NULL;
    psych_bool isDeviceSpecified;

    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(2));

    // Get the optional device index:
    isDeviceSpecified = PsychCopyInIntegerArg(1, FALSE, &deviceIndex);
    if (!isDeviceSpecified) {
        // Set the keyboard or keypad device to be the default keyboard device or, if no keyboard, the first keypad:
        deviceIndex = INT_MAX;
    }

    // Optional 2nd argument 'scanlist' provided?
    if (PsychAllocInDoubleMatArg(2, FALSE, &m, &n, &p, &scanList)) {
        // Yep. Matching size?
        if (p!=1 || m * n != 256) PsychErrorExitMsg(PsychError_user, "Provided 'scanList' parameter is not a vector of 256 doubles, as required!");
    }

    return(PsychHIDOSKbCheck(deviceIndex, scanList));
}
