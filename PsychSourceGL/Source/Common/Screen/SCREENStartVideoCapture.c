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

static char useString[] = "fps = Screen('StartVideoCapture', capturePtr [, captureRateFPS]);";
static char synopsisString[] = "Start video capture device specified by 'capturePtr'. If 'captureRateFPS' "
                               "is provided, the device is requested to capture at that rate. Otherwise it "
                               "is requested to operate at 25 Hz. ";
static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";
	 
PsychError SCREENStartVideoCapture(void) 
{
    int capturehandle = -1;
    double captureFPS = 25;
    
    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};
    PsychErrorExit(PsychCapNumInputArgs(2));            // Max. 2 input args.
    PsychErrorExit(PsychRequireNumInputArgs(1));        // Min. 1 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(1));           // One output arg.
    
    // Get the handle:
    PsychCopyInIntegerArg(1, TRUE, &capturehandle);
    if (capturehandle==-1) {
        PsychErrorExitMsg(PsychError_user, "StartVideoCapture called without valid handle to a capture object.");
    }
    
    PsychCopyInDoubleArg(2, FALSE, &captureFPS);
    if (captureFPS<=0) {
        PsychErrorExitMsg(PsychError_user, "StartVideoCapture called with a negative capture rate.");
    }

    // Try to start capture:
    captureFPS = (double) PsychVideoCaptureRate(capturehandle, captureFPS, 0);

    PsychCopyOutDoubleArg(1, FALSE, captureFPS);

    // Ready!    
    return(PsychError_none);
}
