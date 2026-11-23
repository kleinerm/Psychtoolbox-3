/*

    Psychtoolbox3/Source/Common/SCREENScheduleCallbackOnFlip.c

    AUTHORS:

        alex.forrence@gmail.com     adf

    PLATFORMS:

        This file should build on any platform.

    HISTORY:

        1/16/26  adf    Created.

    DESCRIPTION:

        Schedule functions to run immediately following a synchronous flip for a specific window.
*/

#include "Screen.h"

static char useString[] = "Screen('ScheduleCallbackOnFlip', windowPtr, functionHandle, [...])";

static char synopsisString[] =
"Schedule a function to run immediately following a synchronous flip for a specific window.\n"
"\"windowPtr\" is the window index of the onscreen window to schedule the callback for. "
"\"functionHandle\" is a handle to the function to be called. "
"Timing information for the most recently completed flip is implicitly passed as the first argument "
"to the callback function. That struct contains the same values returned by Screen('Flip'). "
"Additional arguments to the callback function can be passed in the optional [...] arguments.\n"
"Caution: The called function is not allowed to call any Screen() commands or you'll "
"likely get undefined behaviour or a crash -- Screen is not reentrant!";
static char seeAlsoString[] = "ClearFlipCallbacks Flip";

PsychError SCREENScheduleCallbackOnFlip(void)
{
    PsychWindowIndexType windowIndex;
    const PsychGenericScriptType *callback = NULL;
    const PsychGenericScriptType **args = NULL;
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychRequireNumInputArgs(2));
    PsychErrorExit(PsychCapNumOutputArgs(0));

    PsychCopyInWindowIndexArg(1, TRUE, &windowIndex);
    callback = PsychGetInArgPtr(2);
    if (callback == NULL || PsychGetArgType(2) != PsychArgType_functionHandle)
        PsychErrorExitMsg(PsychError_user, "Screen ScheduleCallbackOnFlip: 'functionHandle' arg is not a function handle.");

    int nargs = PsychGetNumInputArgs() - 2;
    if (nargs > 0) {
        args = (PsychGenericScriptType**) PsychMallocTemp(nargs * sizeof(PsychGenericScriptType*));
        for (int i = 0; i < nargs; i++) {
            args[i] = PsychGetInArgPtr(i + 3);
        }
    }
    PsychScheduleCallbackOnFlip(windowIndex, callback, nargs, args);

    if (args)
        PsychFreeTemp(args);

    return(PsychError_none);
}
