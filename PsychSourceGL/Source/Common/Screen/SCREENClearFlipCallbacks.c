/*

    Psychtoolbox3/Source/Common/SCREENClearFlipCallbacks.c

    AUTHORS:

        alex.forrence@gmail.com     adf

    PLATFORMS:

        This file should build on any platform.

    HISTORY:

        1/16/26  adf    Created.

    DESCRIPTION:

        Cancel all pending callback functions for a given window.
*/

#include "Screen.h"

static char useString[] = "Screen('ClearFlipCallbacks', windowPtr)";

static char synopsisString[] =
"Clear pending flip callbacks for a given window.\n"
"\"windowPtr\" is the id of the onscreen window to clear the callbacks for.\n";
static char seeAlsoString[] = "ScheduleCallbackOnFlip";

PsychError SCREENClearFlipCallbacks(void)
{
    PsychWindowIndexType windowIndex;
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychRequireNumInputArgs(1));
    PsychErrorExit(PsychCapNumOutputArgs(0));

    PsychCopyInWindowIndexArg(1, TRUE, &windowIndex);
    PsychClearFlipCallbacks(windowIndex);
    return(PsychError_none);
}
