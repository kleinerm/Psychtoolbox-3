/*
Psychtoolbox3/Source/Common/SCREENGetCapturedImage.c		
 
 AUTHORS:
 mario.kleiner at tuebingen.mpg.de   mk
 
 PLATFORMS:	
 This file should build on any platform. 
 
 HISTORY:
 2/7/06  mk		Created. 
 
 DESCRIPTION:
 
 Fetch an image from the specified capture object and create an OpenGL texture out of it.
 Return a handle to the texture.
 
 TO DO:

 */

#include "Screen.h"

static char useString[] = "[ texturePtr [timeindex] [summed_intensity]]=Screen('GetCapturedImage', windowPtr, capturePtr, [waitForImage=1], [fortimeindex]);";
static char synopsisString[] = 
"Try to fetch a new texture image from video capture device 'capturePtr' for visual playback/display in onscreen window 'windowPtr' and "
"return a texture-handle 'texturePtr' on successfull completion. 'waitForImage' If set to 1 (default), the function will wait "
"until the image becomes available. If set to zero, the function will just poll for a new image. If none is ready, it will return "
"a texturePtr of zero, or -1 if none will become ready because capture has been stopped. Set to 2 if you just want to get the "
"summed intensity of the image. 'fortimeindex' Don't request the next image, but the image closest to fortimeindex. The (optional) return "
"value 'timeindex' contains the exact time when the returned image was captured. The (optional) return "
"value 'summed_intensity' contains the sum of all pixel intensity values of all channels of the image - "
"some measure of overall brightness. Only query this value if you need it, computation is expensive.";

static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";

PsychError SCREENGetCapturedImage(void) 
{
    PsychWindowRecordType		*windowRecord;
    PsychWindowRecordType		*textureRecord;
    PsychRectType			rect;
    double                              summed_intensity;
    int                                 moviehandle = -1;
    int                                 waitForImage = TRUE;
    double                              requestedTimeIndex = -1;
    double                              presentation_timestamp = 0;
    int rc=0;
    
    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};
    
    PsychErrorExit(PsychCapNumInputArgs(4));            // Max. 4 input args.
    PsychErrorExit(PsychRequireNumInputArgs(2));        // Min. 2 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(3));           // Max. 3 output args.
    
    // Get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    // Only onscreen windows allowed:
    if(!PsychIsOnscreenWindow(windowRecord)) {
        PsychErrorExitMsg(PsychError_user, "GetCapturedImage called on something else than an onscreen window.");
    }
    
    // Get the handle:
    PsychCopyInIntegerArg(2, TRUE, &moviehandle);
    if (moviehandle==-1) {
        PsychErrorExitMsg(PsychError_user, "GetCapturedImage called without valid handle to a capture object.");
    }

    // Get the 'waitForImage' flag: If waitForImage == true == 1, we'll do a blocking wait for
    // arrival of a new image. Otherwise we will return with a 0-Handle if there
    // isn't any new image available.
    PsychCopyInIntegerArg(3, FALSE, &waitForImage);
    
    // Get the requested timeindex for the frame. The default is -1, which means: Get the next image
    // in the queue.
    PsychCopyInDoubleArg(4, FALSE, &requestedTimeIndex);
    
    while (rc==0) {
        rc = PsychGetTextureFromCapture(windowRecord, moviehandle, TRUE, requestedTimeIndex, NULL, &presentation_timestamp, NULL);
        if (rc<0) {
            // No image available and there won't be any in the future, because capture has been stopped.

            // No new texture available: Return a negative handle:
            PsychCopyOutDoubleArg(1, TRUE, -1);
            // ...and an invalid timestamp:
            PsychCopyOutDoubleArg(2, FALSE, -1);
            PsychCopyOutDoubleArg(3, FALSE, 0);
            // Ready!
            return(PsychError_none);
        }
        else if (rc==0 && waitForImage == 0) {
            // We should just poll once and no new texture available: Return a null-handle:
            PsychCopyOutDoubleArg(1, TRUE, 0);
            // ...and the current timestamp:
            PsychCopyOutDoubleArg(2, FALSE, presentation_timestamp);
            PsychCopyOutDoubleArg(3, FALSE, 0);
            // Ready!
            return(PsychError_none);
        }
        else if (rc==0 && waitForImage != 0) {
            // No new texture available yet. Just sleep a bit and then retry...
            PsychWaitIntervalSeconds(0.005);
        }
    }

    // New image available: Go ahead...
    if (waitForImage!=2) {
        // Create a texture record.  Really just a window record adapted for textures.  
        PsychCreateWindowRecord(&textureRecord);	// This also fills the window index field.
                                                        // Set mode to 'Texture':
        textureRecord->windowType=kPsychTexture;
        // We need to assign the screen number of the onscreen-window.
        textureRecord->screenNumber=windowRecord->screenNumber;
        // It is always a 32 bit texture for captured images:
        textureRecord->depth=32;
        
        // Create default rectangle which describes the dimensions of the image. Will be overwritten
        // later on.
        PsychMakeRect(rect, 0, 0, 10, 10);
        PsychCopyRect(textureRecord->rect, rect);
        
        // Other setup stuff:
        textureRecord->textureMemorySizeBytes= 0;
        textureRecord->textureMemory=NULL;
        
        // Assign proper OpenGL-Renderingcontext to texture:
        // MK: Is this the proper way to do it???
        textureRecord->targetSpecific.contextObject = windowRecord->targetSpecific.contextObject;
        textureRecord->targetSpecific.deviceContext = windowRecord->targetSpecific.deviceContext;
    }
    else {
        // Just want to return summed_intensity and timestamp, not real texture...
        textureRecord = NULL;
    }

    // Try to fetch an image from the movie object and return it as texture:
    if (PsychGetNumOutputArgs() > 2) {
        // Return sum of pixel intensities for all channels of this image:
        PsychGetTextureFromCapture(windowRecord, moviehandle, FALSE, requestedTimeIndex, textureRecord, &presentation_timestamp, &summed_intensity);
        PsychCopyOutDoubleArg(3, FALSE, summed_intensity);
    }
    else {
        PsychGetTextureFromCapture(windowRecord, moviehandle, FALSE, requestedTimeIndex, textureRecord, &presentation_timestamp, NULL);
    }

    // Real texture requested?
    if (textureRecord) {
        // Texture ready for consumption. Mark it valid and return handle to userspace:
        PsychSetWindowRecordValid(textureRecord);
        PsychCopyOutDoubleArg(1, TRUE, textureRecord->windowIndex);
    }
    else {
        PsychCopyOutDoubleArg(1, TRUE, 0);
    }
    
    // Return presentation timestamp for this image:
    PsychCopyOutDoubleArg(2, FALSE, presentation_timestamp);

    // Ready!
    return(PsychError_none);
}
