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

static char useString[] = "droppedframes = Screen('StopVideoCapture', capturePtr [, discardFrames=1]);";
static char synopsisString[] = "Stop video capture device specified by 'capturePtr'. The function returns the "
                               "number of captured frames it had to drop from its internal buffers in order to "
                               "keep synchronization with realtime or because it ran out of internal buffer space.\n"
			       "If the optional 'discardFrames' flag is set to zero, then captured but not yet "
			       "fetched video images will not get deleted. Instead they are retained until "
			       "next time 'StartVideoCapture' is called. This allows to fetch them after "
			       "capture has been stopped. By default, all pending frames are deleted at "
			       "stop of capture (discardFrames == 1).\n";

static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";

PsychError SCREENStopVideoCapture(void) 
{
    int capturehandle = -1;
    int dropped, discardFrames;
    
    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};
    PsychErrorExit(PsychCapNumInputArgs(2));            // Max. 2 input args.
    PsychErrorExit(PsychRequireNumInputArgs(1));        // Min. 1 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(1));           // One output arg.
    
    // Get the handle:
    PsychCopyInIntegerArg(1, TRUE, &capturehandle);
    if (capturehandle==-1) {
        PsychErrorExitMsg(PsychError_user, "StopVideoCapture called without valid handle to a capture object.");
    }

    // Get the optional 'discardFrames' flag:
    discardFrames = 1;
    PsychCopyInIntegerArg(2, FALSE, &discardFrames);
    if ((discardFrames < 0) || (discardFrames > 1)) {
        PsychErrorExitMsg(PsychError_user, "StopVideoCapture called with invalid 'discardFrames' flag (must be 0 or 1)!");
    }
    
    // Try to stop capture:
    dropped = PsychVideoCaptureRate(capturehandle, 0, discardFrames, NULL);

    PsychCopyOutDoubleArg(1, FALSE, dropped);
    
    // Ready!    
    return(PsychError_none);
}
