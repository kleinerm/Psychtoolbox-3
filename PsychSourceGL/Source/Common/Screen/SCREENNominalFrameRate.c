/*
 *        Psychtoolbox3/Source/Common/SCREENNominalFrameRate.c
 *
 *        AUTHORS:
 *
 *                Allen.Ingling@nyu.edu           awi
 *                mario.kleiner.de@gmail.com      mk
 *
 *        PLATFORMS:
 *
 *                All systems.
 *
 *        HISTORY:
 *
 *                12/04/02  awi         Created.
 *                06/07/06  mk          Extended to return float precision values on request.
 *                                      Extended to allow setting the framerate on systems that
 *                                      support this (Currently Linux only).
 *
 *        DESCRIPTION:
 *
 *                Return the the nominal frame rate as reported by the video subsystem/driver, or
 *                change the nominal frame rate on systems that support this.
 */

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "hz=Screen('NominalFrameRate', windowPtrOrScreenNumber [, mode=0][, reqFrameRate]);";
//                         1                             1                          2          3
static char synopsisString[] =
    "Returns or sets the nominal video frame rate in Hz, as reported by your computer's video driver.\n"
    "'FrameRate' is an alias for 'NominalFrameRate'. By default, this function returns the nominal "
    "framerate, as reported by your operating system, rounded to the closest integral value. If you "
    "set the optional 'mode' flag to 1, then the framerate is returned without rounding to the closest "
    "integer, but at floating point precision, on systems that support this (macOS and Linux).\n"
    "GNU/Linux/X11 only: If you set the 'mode' flag to 2 and specify 'reqFrameRate', then Screen will "
    "try to quickly switch to a framerate as close as possible to 'reqFrameRate' Hz. Invalid settings are "
    "rejected, returning a value of -1. On failure, 0 is returned to signal inability to switch refresh. "
    "On systems other than Linux, 0 is always returned to signal failure. On successfull framerate "
    "change the new resulting nominal framerate is returned with double precision. NOTE: This function "
    "allows fast and almost seamless video refresh rate switching on AMD graphics cards when used to drive "
    "a FreeSync capable display device. The 'reqFrameRate' should be within the supported FreeSync refresh "
    "rate range of your display device, or this function may silently fail! On other graphics cards or on "
    "standard fixed refresh rate displays, this may or may not work, but it will likely result in a time "
    "consuming video modesetting operation, during which the display may go blank for up to multiple seconds.\n"
    "One limitation at the moment is that in a multi-display setup, your visual stimulation window must be "
    "fully or at least predominantly covering the display area of the primary video output monitor, as the "
    "refresh rate of that monitor will be assigned for Screen('Flip', ...) scheduling of stimulus onset time. "
    "Otherwise only immediate flips at each video refresh cycle will work with reliable timing. "
    "See 'help VRRSupport' for possibly more background and setup info.\n"
    "Due to manufacturing tolerances and other noise in your system, the real monitor refresh interval can "
    "differ slightly from the nominal values returned by this function. To query the real, measured framerate "
    "use Screen('GetFlipInterval') instead. ";

static char seeAlsoString[] = "FrameRate GetFlipInterval";

PsychError SCREENNominalFramerate(void)
{
    PsychWindowRecordType *windowRecord;
    int screenNumber, opmode;
    double *rate;
    double requestedHz;

    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(3));

    //get specified screen number and sanity check the number against the number of connected displays.
    PsychCopyInScreenNumberArg(kPsychUseDefaultArgPosition, TRUE, &screenNumber);

    // Get opmode. Defaults to zero for integral precision query:
    opmode = 0;
    PsychCopyInIntegerArg(2, FALSE, &opmode);

    // Allocate a return matrix.
    PsychAllocOutDoubleArg(1, FALSE, &rate);

    // Backend override active for external display backend?
    if (PsychIsWindowIndexArg(1) &&
        PsychAllocInWindowRecordArg(1, TRUE, &windowRecord) &&
        (windowRecord->imagingMode & kPsychNeedFinalizedFBOSinks)) {
        // Yes: Report "made up" values injected from external backend, instead
        // of the ones associated with this windows windowing system screen:
        *rate = 1.0 / windowRecord->VideoRefreshInterval;

        // Round it to closest integer, if opmode <= 0:
        if (opmode <= 0) *rate = (double) ((int) (*rate + 0.5));

        return(PsychError_none);
    }

    // Query mode (0 or 1)?
    if (opmode<=1) {
        // Query the float precision nominal frame rate and put it into the return value:
        *rate= (double) PsychGetNominalFramerate(screenNumber);
    }
    else {
        // Query new requested framerate or mode increment:
        PsychCopyInDoubleArg(3, TRUE, &requestedHz);

        // Set mode: This is currently only supported on GNU/Linux:
        #if PSYCH_SYSTEM == PSYCH_LINUX
        // Call the Set-function, it will process and return the new nominal framerate:
        *rate = PsychSetNominalFramerate(screenNumber, requestedHz);
        #else
        // Return 0 to signal the unsupported feature:
        *rate = 0;
        #endif
    }

    // Round it to closest integer, if opmode <= 0:
    if (opmode <= 0) *rate = (double) ((int) (*rate + 0.5));

    // Done.
    return(PsychError_none);
}
