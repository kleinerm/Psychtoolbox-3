/*

 Psychtoolbox3/Source/Common/SCREENCloseVideoCapture.c		

 AUTHORS:

 mario.kleiner at tuebingen.mpg.de   mk

 PLATFORMS:	

 This file should build on any platform. 

 HISTORY:
 2/7/06  mk		Created. 

 DESCRIPTION:

 Close a previously opened video capture device and release all associated ressources.

 TO DO:
 
 */

#include "Screen.h"

static char useString[] = "Screen('CloseVideoCapture', capturePtr);";
static char synopsisString[] = "Close video capture device specified by 'capturePtr' and release all associated ressources.";
static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";

PsychError SCREENCloseVideoCapture(void) 
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
        PsychErrorExitMsg(PsychError_user, "CloseVideoCapture called without valid handle to a capture object.");
    }

    // Try to delete the object, releasing all associated ressources:
    PsychCloseVideoCaptureDevice(capturehandle);

    // Ready!    
    return(PsychError_none);
}










