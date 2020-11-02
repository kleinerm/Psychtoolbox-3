/*

  Psychtoolbox3/Source/Common/SCREENScreens.c

  AUTHORS:

  Allen.Ingling@nyu.edu                 awi 
  mario.kleiner.de@gmail.com            mk

  PLATFORMS:

  This file should build on any platform. 

  HISTORY:

    11/26/02    awi        Created. 
    10/12/04    awi        Changed "SCREEN" to "Screen" in useString.

  DESCRIPTION:

  Return the number of screens.

*/

#include "Screen.h"
#include <ctype.h>

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "screenNumbers=Screen('Screens' [, physicalDisplays]);";
static char synopsisString[] = 
        "Return an array of screenNumbers, corresponding to available logical or physical displays.\n"
        "Please note that the set of reported screens is only refreshed on first invocation of Screen() "
        "after application startup or after a 'clear all'. If you replug or en-/disable displays, "
        "you must execute 'clear all' to force a redetection of available displays. This is done for "
        "efficiency reasons to get better realtime behaviour.\n"
        "The meaning/mapping of screenNumbers to physical display devices on different operating "
        "systems differs:\n"
        "On Linux with X11 X-Server display system, screenNumber 0 - n correspond to X-Screens 0 - n. "
        "As a single X-Screen can have multiple physical displays assigned, there isn't a one to one "
        "mapping by default. E.g., by default, all displays are assigned to X-Screen 0, so a multi-display "
        "setup will only report one screenNumber, the number 0. You can define arbitrary mappings with "
        "our setup script XOrgConfCreator, to optimally suit your needs.\n"
        "If this command is executed on Microsoft Windows in a multi-display configuration, then the "
        "following rule applies: Screen 0 is always the first two monitors assigned to the Windows "
        "desktop. Screens 1 to n are corresponding to windows display monitors 1 to n. If "
        "you want to open an onscreen window only on one specific display, or you want to query or "
        "set the properties of a display (e.g., framerate, size, color depth or gamma tables), use "
        "the screen numbers 1 to n. If you want to open a window suitable for stereo display on a "
        "dual display setup consisting of the first two monitors, use screen zero.\n"
        "On OSX, numbers 0 - n correspond to logical displays 0 - n. "
        "When executed on OSX with the optional 'physicalDisplays' flag set to 1, it will enumerate "
        "the set of physical displays, which can be different from the set of logical displays "
        "that is returned by default. E.g., in mirror mode or clone mode, there is only one logical "
        "display, representing the mirror set of all mirrored physical displays. This is mostly useful "
        "to set, e.g., gamma tables on a per display basis, even if in mirror mode. Note that according "
        "to some user reports, reporting of the true set of physical displays may have been broken "
        "by Apple in current macOS versions: The set of reported logical and physical displays may "
        "be the same even in mirror mode configurations, due to macOS operating system defects.\n";

static char seeAlsoString[] = "";

PsychError SCREENScreens(void) 
{
    int i, numDisplays;
    double *displayNumList;
    static psych_bool firstTime = TRUE;
    int physicalDisplays = 0;
    int startidx = 0;
    const char* screens = getenv("PSYCH_REPORTED_SCREENS");

    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Copy in optional flag:
    PsychCopyInIntegerArg(1, FALSE, &physicalDisplays);

    if (PSYCH_SYSTEM == PSYCH_OSX && physicalDisplays == 1) {
        // We have one-based indexing on OS/X for physical displays:
        startidx = 1;

        #if PSYCH_SYSTEM == PSYCH_OSX
            // Get the number of connected physical displays:
            numDisplays = PsychGetNumPhysicalDisplays();
        #endif
    }
    else {
        // Get the number of connected logical displays:
        numDisplays = PsychGetNumDisplays();
    }

    // Override list of Screens to report provided as environment variable?
    if (!screens || !strlen(screens)) {
        // No: Allocate an output matrix.  Even if argument is not there, we still get the space.
        PsychAllocOutDoubleMatArg(1, FALSE, 1, numDisplays, 0, &displayNumList);

        // Fill the return matrix
        for (i = 0; i < numDisplays; i++)
            displayNumList[i] = i + startidx;
    }
    else {
        // Yes: Return that list instead for special diagnostic/debugging:
        numDisplays = strlen(screens);
        PsychAllocOutDoubleMatArg(1, FALSE, 1, numDisplays, 0, &displayNumList);

        // Fill the return matrix until reaching a non-digit character, ie. a
        // number other than 0 to 9:
        for (i = 0; (i < numDisplays) && isdigit(screens[i]); i++)
            displayNumList[i] = screens[i] - 0x30; // 0x30 is ASCII digit 0

        // Report override condition, but only on presumably first invocation in script, before any windows are open:
        if ((PsychPrefStateGet_Verbosity() > 2) && firstTime) {
            firstTime = FALSE;
            printf("PTB-INFO: Screen('Screens') reports overridden set of screens from getenv('PSYCH_REPORTED_SCREENS').\n");
        }
    }

    return(PsychError_none);
}
