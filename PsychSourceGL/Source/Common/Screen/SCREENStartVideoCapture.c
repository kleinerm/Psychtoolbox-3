/*
 
 Psychtoolbox3/Source/Common/SCREENStartVideoCapture.c		
 
 AUTHORS:
 
 mario.kleiner at tuebingen.mpg.de   mk
 
 PLATFORMS:	
 
 This file should build on any platform. 
 
 HISTORY:
 2/7/06  mk		Created. 
 
 DESCRIPTION:
 
 Start a previously opened video capture device.
 
 TO DO:
 
 */

#include "Screen.h"

static char useString[] = "Screen('StartVideoCapture', capturePtr);";
static char synopsisString[] = "Start video capture device specified by 'capturePtr'.";
static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";
	 
PsychError SCREENStartVideoCapture(void) 
{
    int capturehandle = -1;
    
    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};
    PsychErrorExit(PsychCapNumInputArgs(1));            // Max. 1 input args.
    PsychErrorExit(PsychRequireNumInputArgs(1));        // Min. 1 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(0));           // No output args.
    
    // Get the handle:
    PsychCopyInIntegerArg(1, TRUE, &capturehandle);
    if (capturehandle==-1) {
        PsychErrorExitMsg(PsychError_user, "StartVideoCapture called without valid handle to a capture object.");
    }
    
    // Try to start capture:
    PsychVideoCaptureRate(capturehandle, 1, 0);

    // Ready!    
    return(PsychError_none);
}
