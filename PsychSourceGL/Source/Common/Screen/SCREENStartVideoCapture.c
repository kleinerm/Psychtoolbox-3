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

static char useString[] = "fps = Screen('StartVideoCapture', capturePtr [, captureRateFPS=25] [, dropframes=0]);";
static char synopsisString[] = "Start video capture device specified by 'capturePtr'. If 'captureRateFPS' "
                               "is provided, the device is requested to capture at that rate. Otherwise it "
                               "is requested to operate at 25 Hz. The real capture rate may differ from the "
                               "requested one, depending on the capabilities of your capture device. PTB tries "
                               "to select the closest supported framerate that is at least as high as the requested "
                               "one. The real capture rate is returned as return argument 'fps'. "
                               "If 'dropframes' is provided and set to 1, "
                               "the device is requested to always deliver the most recently acquired frame, "
                               "dropping previously captured but not delivered frames if necessary. The default "
                               "is to queue up as many frames as possible. If you want to do video recordings, you "
                               "want to have the default of zero. If you want to do interactive realtime processing "
                               "of video data (e.g, video feedback for action-perception studies or build your own "
                               "low-cost eyetracker), then you want to use dropframes=1 for lowest possible latency. "
                               "'dropframes' is currently silently ignored on Windows and OS-X, only Linux supports this.";
static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";
	 
PsychError SCREENStartVideoCapture(void) 
{
    int capturehandle = -1;
    double captureFPS = 25;
    int dropframes = 0;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};
    PsychErrorExit(PsychCapNumInputArgs(3));            // Max. 3 input args.
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

    PsychCopyInIntegerArg(3, FALSE, &dropframes);
    if (dropframes<0) {
        PsychErrorExitMsg(PsychError_user, "StartVideoCapture called with invalid (negative) dropframes - argument.");
    }

    // Try to start capture:
    captureFPS = (double) PsychVideoCaptureRate(capturehandle, captureFPS, dropframes);

    PsychCopyOutDoubleArg(1, FALSE, captureFPS);

    // Ready!    
    return(PsychError_none);
}
