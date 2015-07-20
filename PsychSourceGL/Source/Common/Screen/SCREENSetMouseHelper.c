/*
    SCREENSetMouseHelper.c

    AUTHORS:

        Allen.Ingling@nyu.edu       awi
        mario.kleiner.de@gmail.com  mk

    PLATFORMS:

        All.

    HISTORY:

        12/14/02    awi     Created.
        10/12/04    awi     Changed "SCREEN" to "Screen" in useString.
        11/16/04    awi     Added "Helper" suffix.
        11/18/04    awi     Renamed to SetMousHelper from PositionCursorHelper

    DESCRIPTION:

        Position the mouse pointer

    TO DO:

        'detachFromMouse' for Linux and Windows.
*/

#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('SetMouseHelper', windowPntrOrScreenNumber, x, y [, mouseIndex][, detachFromMouse]);";
//                                                  1                         2  3  4               5
static char synopsisString[] =
    "This is a helper function called by SetMouse.  Do not call Screen(\'SetMouseHelper\'), use "
    "SetMouse instead.\n"
    "Move the mouse pointer, also potentially detach it from the mouse on "
    "supported systems, currently OSX.";
static char seeAlsoString[] = "";

PsychError SCREENSetMouseHelper(void)
{
    int	screenNumber;
    int xPos, yPos;
    int deviceIdx = -1;
    int detachFromMouse = 0;

    //all subfunctions should have these two lines.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumInputArgs(5));   //The maximum number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs

    PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
    PsychCopyInIntegerArg(2,TRUE, &xPos);
    PsychCopyInIntegerArg(3,TRUE, &yPos);
    PsychCopyInIntegerArg(4,FALSE, &deviceIdx);
    PsychCopyInIntegerArg(5,FALSE, &detachFromMouse);

    // Position the cursor. On OSX this will also automatically attach it
    // to mouse movement:
    PsychPositionCursor(screenNumber, xPos, yPos, deviceIdx);

    #if PSYCH_SYSTEM == PSYCH_OSX
        if (detachFromMouse) CGAssociateMouseAndMouseCursorPosition(false);
    #endif

    return(PsychError_none);
}
