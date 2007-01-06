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

static char useString[] = "[ texturePtr [capturetimestamp] [droppedcount] [summed_intensity]]=Screen('GetCapturedImage', windowPtr, capturePtr [, waitForImage=1] [,oldTexture] [,specialmode]);";
static char synopsisString[] = 
"Try to fetch a new image from video capture device 'capturePtr' for visual playback/display in window 'windowPtr' and "
"return a texture-handle 'texturePtr' on successfull completion. 'waitForImage' If set to 1 (default), the function will wait "
"until the image becomes available. If set to zero, the function will just poll for a new image. If none is ready, it will return "
"a texturePtr of zero, or -1 if none will become ready because capture has been stopped. Set 'waitForImage' = 2 if you just want to get the "
"summed intensity or timestamp of the image. 'oldTexture' (if provided) allows you to pass the texture handle of an already existing texture. "
"Psychtoolbox will reuse that texture by overwriting its previous image content with the new image, instead of creating a completely new "
"texture for the new image. Use of this ''recycling facility'' may allow for higher capture framerates and lower latencies on low-end "
"graphics hardware in some cases. Providing a value of 'oldTexture'=0 is the same as leaving it out. The optional argument 'specialmode' "
"allows to request special treatment of textures. Currently, specialmode = 1 will force PTB to use power-of-two textures instead of other "
"formats. This is usually less efficient, unless you want to do realtime blurring of images. "
"'capturetimestamp' contains the exact system time when the returned image was captured. The (optional) return value 'droppedcount' contains the "
"number of captured frames that had to be dropped to keep in sync with realtime or due to internal shortage of buffer memory. The (optional) return "
"value 'summed_intensity' contains the sum of all pixel intensity values of all channels of the image - some measure of overall brightness. "
"Only query this value if you really need it, its computation is time consuming.";

static char seeAlsoString[] = "CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";

PsychError SCREENGetCapturedImage(void) 
{
    PsychWindowRecordType		*windowRecord;
    PsychWindowRecordType		*textureRecord;
    PsychRectType			rect;
    double                              summed_intensity;
    int                                 capturehandle = -1;
    int                                 waitForImage = TRUE;
    int                                 specialmode = 0;
    double                              presentation_timestamp = 0;
    int rc=-1;
    
    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};
    
    PsychErrorExit(PsychCapNumInputArgs(5));            // Max. 5 input args.
    PsychErrorExit(PsychRequireNumInputArgs(2));        // Min. 2 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(4));           // Max. 4 output args.
    
    // Get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    // Only onscreen windows allowed:
    if(!PsychIsOnscreenWindow(windowRecord) && !PsychIsOffscreenWindow(windowRecord)) {
        PsychErrorExitMsg(PsychError_user, "GetCapturedImage called on something else than an onscreen window or offscreen window.");
    }
    
    // Get the handle:
    PsychCopyInIntegerArg(2, TRUE, &capturehandle);
    if (capturehandle==-1) {
        PsychErrorExitMsg(PsychError_user, "GetCapturedImage called without valid handle to a capture object.");
    }

    // Get the 'waitForImage' flag: If waitForImage == true == 1, we'll do a blocking wait for
    // arrival of a new image. Otherwise we will return with a 0-Handle if there
    // isn't any new image available.
    PsychCopyInIntegerArg(3, FALSE, &waitForImage);

    // Get the optional textureRecord for the optional texture handle. If the calling script
    // provides the texture handle of an existing Psychtoolbox texture that has a matching
    // format, then that texture is recycled by overwriting its previous content with the
    // image data from the new captured image. This can save some overhead for texture destruction
    // and recreation. While this is probably not noticeable on mid- to high-end gfx cards with
    // rectangle texture support, it can provide a significant speedup on low-end gfx cards with
    // only power-of-two texture support.
    textureRecord = NULL;
    if ((PsychGetNumInputArgs()>=4) && PsychIsWindowIndexArg(4)) PsychAllocInWindowRecordArg(4, FALSE, &textureRecord);
    
    // Get the optional specialmode flag:
    PsychCopyInIntegerArg(5, FALSE, &specialmode);

    while (rc==-1) {
      //  We pass a checkForImage value of 2 if waitForImage>0. This way we can signal if we are in polling or blocking mode.
      // On Linux this allows to do a real blocking wait in the driver -- much more efficient than the spin-waiting approach!
      rc = PsychGetTextureFromCapture(windowRecord, capturehandle, ((waitForImage>0) ? 2 : 1), 0.0, NULL, &presentation_timestamp, NULL);
        if (rc==-2) {
            // No image available and there won't be any in the future, because capture has been stopped.

            // No new texture available: Return a negative handle:
            PsychCopyOutDoubleArg(1, TRUE, -1);
            // ...and an invalid timestamp:
            PsychCopyOutDoubleArg(2, FALSE, -1);
            PsychCopyOutDoubleArg(3, FALSE, 0);
            PsychCopyOutDoubleArg(4, FALSE, 0);

            // Ready!
            return(PsychError_none);
        }
        else if (rc==-1 && waitForImage == 0) {
            // We should just poll once and no new texture available: Return a null-handle:
            PsychCopyOutDoubleArg(1, TRUE, 0);
            // ...and the current timestamp:
            PsychCopyOutDoubleArg(2, FALSE, presentation_timestamp);
            PsychCopyOutDoubleArg(3, FALSE, 0);
            PsychCopyOutDoubleArg(4, FALSE, 0);

            // Ready!
            return(PsychError_none);
        }
        else if (rc==-1 && waitForImage != 0) {
            // No new texture available yet. Just sleep a bit and then retry...
            PsychWaitIntervalSeconds(0.005);
        }
    }

    // rc == 0 --> New image available: Go ahead...
    if (waitForImage!=2) {
      // Ok, we need a texture for the image. Did script provide an old one for recycling?
      if (textureRecord) {
	// Old texture provided for reuse? Some basic sanity check: Everything else is
	// up to the lower level PsychGetTextureFromCapture() routine.
        if(!PsychIsOffscreenWindow(textureRecord)) {
	  PsychErrorExitMsg(PsychError_user, "GetCapturedImage provided with something else than a texture as fourth call parameter.");
        }	
      }
      else {
        // No old texture provided: Create a new texture record:
        PsychCreateWindowRecord(&textureRecord);

        // Set mode to 'Texture':
        textureRecord->windowType=kPsychTexture;

        // We need to assign the screen number of the onscreen-window.
        textureRecord->screenNumber=windowRecord->screenNumber;

        // It defaults to a 32 bit texture for captured images. On Linux, this will be overriden,
		// if optimized formats exist for our purpose:
        textureRecord->depth=32;
		textureRecord->nrchannels = 4;

        // Create default rectangle which describes the dimensions of the image. Will be overwritten
        // later on.
        PsychMakeRect(rect, 0, 0, 10, 10);
        PsychCopyRect(textureRecord->rect, rect);
        
        // Other setup stuff:
        textureRecord->textureMemorySizeBytes= 0;
        textureRecord->textureMemory=NULL;
        
        // Assign proper OpenGL-Renderingcontext to texture:
        textureRecord->targetSpecific.contextObject = windowRecord->targetSpecific.contextObject;
        textureRecord->targetSpecific.deviceContext = windowRecord->targetSpecific.deviceContext;
        textureRecord->targetSpecific.glusercontextObject = windowRecord->targetSpecific.glusercontextObject;

		textureRecord->colorRange = windowRecord->colorRange;
		// Copy imaging mode flags from parent:
		textureRecord->imagingMode = windowRecord->imagingMode;

        // Set textureNumber to zero, which means "Not cached, do not recycle"
        // Todo: Texture recycling like in PsychMovieSupport for higher efficiency!
        textureRecord->textureNumber = 0;
      }

      // Power-of-two texture requested?
      if (specialmode & 0x01) {
	// Yes. Spec it:
	textureRecord->texturetarget = GL_TEXTURE_2D;
      }
    }
    else {
        // Just want to return summed_intensity and timestamp, not real texture...
        textureRecord = NULL;
    }

    // Try to fetch an image from the capture object and return it as texture:
    if (PsychGetNumOutputArgs() > 3) {
        // Return sum of pixel intensities for all channels of this image:
        rc = PsychGetTextureFromCapture(windowRecord, capturehandle, 0, 0.0, textureRecord, &presentation_timestamp, &summed_intensity);
        PsychCopyOutDoubleArg(4, FALSE, summed_intensity);
    }
    else {
        rc = PsychGetTextureFromCapture(windowRecord, capturehandle, 0, 0.0, textureRecord, &presentation_timestamp, NULL);
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

    // Return count of pending frames in buffers or of dropped frames:
    PsychCopyOutDoubleArg(3, FALSE, (double) rc);

    // Ready!
    return(PsychError_none);
}

