/*
  Psychtoolbox3/Source/Common/SCREENOpenVideoCapture.c		
  
  AUTHORS:
    mario.kleiner at tuebingen.mpg.de   mk
  
  PLATFORMS:	
  This file should build on any platform. 

  HISTORY:
  2/7/06  mk		Created. 
 
  DESCRIPTION:
  
  Open a video capture source (framegrabber, webcam, ...), create and initialize a corresponding capture object
  and return a handle to it to MATLAB space.
 
  On OS-X and Windows, video capture is implemented via Apples Sequence-Grabber API which is part of Quicktime.
  On Linux, we would use a different engine.
  
  TO DO:
  
  
  

*/


#include "Screen.h"


static char useString[] = "[ videoPtr [fps] [width] [height]]=Screen('OpenVideoCapture', windowPtr [, deviceIndex] [roirectangle]);";
static char synopsisString[] = 
	"Try to open the video source 'deviceIndex' for capture into onscreen window 'windowPtr' and "
        "return a handle 'videoPtr' on success. On OS-X and Windows, video capture is handled by use of "
        "Apples Quicktime Sequence-Grabber API. On other platforms, the engine may be different. "
        "The following properties are optionally returned: "
        "'fps' Video capture framerate. "
        "'width' Width of the captured images. 'height' Height of the images. ";

static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";

PsychError SCREENOpenVideoCapture(void) 
{
        PsychWindowRecordType			*windowRecord;
        int                                     deviceIndex;
        int                                     capturehandle = -1;
        double                                  framerate;
        int                                     width;
        int                                     height;
        PsychRectType                           roirectangle;
        Boolean                                 roiassigned;
        
	// All sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

        PsychErrorExit(PsychCapNumInputArgs(3));            // Max. 3 input args.
        PsychErrorExit(PsychRequireNumInputArgs(1));        // Min. 1 input args required.
        PsychErrorExit(PsychCapNumOutputArgs(4));           // Max. 4 output args.
        
        // Get the window record from the window record argument and get info from the window record
        PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
        // Only onscreen windows allowed:
        if(!PsychIsOnscreenWindow(windowRecord)) {
            PsychErrorExitMsg(PsychError_user, "OpenVideoCapture called on something else than an onscreen window.");
        }
        
        // Get the device index. We default to the first device if none is given:
        deviceIndex=0;
        PsychCopyInIntegerArg(2, FALSE, &deviceIndex);

        // Get the optional roi rectangle:
        roiassigned = PsychCopyInRectArg(3, FALSE, roirectangle);

	// Try to open the capture device and create & initialize a corresponding capture object.
        // A MATLAB handle to the movie object is returned upon successfull operation.
        if (roiassigned) {
            PsychOpenVideoCaptureDevice(windowRecord, deviceIndex, &capturehandle, roirectangle);
        }
        else {
            PsychOpenVideoCaptureDevice(windowRecord, deviceIndex, &capturehandle, NULL);
        }
        
        // Upon sucessfull completion, we'll have a valid handle in 'capturehandle'.
        // Return it to Matlab-world:
        PsychCopyOutDoubleArg(1, TRUE, (double) capturehandle);

        // Retrieve infos about new device:
        
 /*       // Is the "count" output argument (total number of frames) requested by user?
        if (PsychGetNumOutputArgs() > 5) {
            // Yes. Query the framecount (expensive!) and return it:
            PsychGetMovieInfos(moviehandle, &width, &height, &framecount, &durationsecs, &framerate, NULL);
            PsychCopyOutDoubleArg(6, TRUE, (double) framecount);
        }
        else {
            // No. Don't compute and return it.
            PsychGetMovieInfos(moviehandle, &width, &height, NULL, &durationsecs, &framerate, NULL);
        }

        PsychCopyOutDoubleArg(2, FALSE, (double) durationsecs);
*/
        PsychCopyOutDoubleArg(2, FALSE, (double) framerate);
        PsychCopyOutDoubleArg(3, FALSE, (double) width);
        PsychCopyOutDoubleArg(4, FALSE, (double) height);

	// Ready!
        return(PsychError_none);
}




