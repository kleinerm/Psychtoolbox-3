/*
 
 Psychtoolbox3/Source/Common/SCREENStopVideoCapture.c		
 
 AUTHORS:
 
 mario.kleiner at tuebingen.mpg.de   mk
 
 PLATFORMS:	
 
 This file should build on any platform. 
 
 HISTORY:
 2/7/06  mk		Created. 
 
 DESCRIPTION:
 
 Stop a previously started video capture device.
 
 TO DO:
 
 */

#include "Screen.h"

static char useString[] = "droppedframes = Screen('StopVideoCapture', capturePtr);";
static char synopsisString[] = "Stop video capture device specified by 'capturePtr'.";
static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";

PsychError SCREENStopVideoCapture(void) 
{
    int capturehandle = -1;
    int dropped;
    
    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};
    PsychErrorExit(PsychCapNumInputArgs(1));            // Max. 1 input args.
    PsychErrorExit(PsychRequireNumInputArgs(1));        // Min. 1 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(1));           // One output arg.
    
    // Get the handle:
    PsychCopyInIntegerArg(1, TRUE, &capturehandle);
    if (capturehandle==-1) {
        PsychErrorExitMsg(PsychError_user, "StopVideoCapture called without valid handle to a capture object.");
    }
    
    // Try to stop capture:
    dropped = PsychVideoCaptureRate(capturehandle, 0, 0);

    PsychCopyOutDoubleArg(1, FALSE, dropped);
    
    // Ready!    
    return(PsychError_none);
}
