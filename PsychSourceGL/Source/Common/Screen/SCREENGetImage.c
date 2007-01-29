/*
	SCREENGetImage.c		
  
	AUTHORS:
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
	
		Only OS X for now.
    
	HISTORY:
	
		01/08/03  	awi		Created.
		10/12/04	awi		In useString: moved commas to inside [].

 
	TO DO:
    
		We could probably speed this up by using a single call to glReadPixels instead of three.  It doesn't matter what we pack the result into,
		since we have to iterate over and copy out from the memory which glReadPixels fills so to rearrange correclty into the return matrix. None
		of the glReadPixels modes will fill the return matrix in the same format that MATLAB wants.  
*/



#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] =  "imageArray=Screen('GetImage', windowPtr [,rect] [,bufferName])";
//                                                        1           2       3
static char synopsisString[] =
	"Slowly copy an image from a window to Matlab, returning a Matlab uint8 array. "
	"The returned imageArray has three layers, i.e. it's an RGB image. "
	"\"rect\" is in window coordinates, and its default is the whole window. "
	"Matlab will complain if you try to do math on a uint8 array, so you may need "
	"to use DOUBLE to convert it, e.g. imageArray/255 will produce an error, but "
	"double(imageArray)/255 is ok. Also see Screen 'PutImage' and 'CopyWindow'. "
	"\"bufferName\" is a string specifying the buffer from which to copy the image: "
        "The 'bufferName' argument is meaningless for Offscreen windows and textures and "
        "will be silently ignored. For Onscreen windows, it defaults to 'frontBuffer', i.e., "
        "it returns the image that your subject would see at that moment. If frame-sequential "
        "stereo mode is enabled, 'frontLeftBuffer' returns what your subject would see in its "
        "left eye, 'frontRightBuffer' returns the subjects right-eye view. If double-buffering "
        "is enabled, you can also return the 'backBuffer', i.e. what your subject will see after "
        "the next Screen('Flip') command, and for frame-sequential stereo also 'backLeftBuffer' "
        "and 'backRightBuffer' respectively. 'aux0Buffer' - 'aux3Buffer' returns the content of "
        "OpenGL AUX buffers 0 to 3. Only query the AUX buffers if you know what you are doing, "
        "otherwise your script will crash, this is mostly meant for internal debugging of PTB. ";

static char seeAlsoString[] = "PutImage";
	

PsychError SCREENGetImage(void) 
{
	PsychRectType 		windowRect,sampleRect;
        int 			ix, iy, sampleRectWidth, sampleRectHeight, invertedY, redReturnIndex, greenReturnIndex, blueReturnIndex, planeSize;
        ubyte 			*returnArrayBase, *redPlane, *greenPlane, *bluePlane;
        PsychWindowRecordType	*windowRecord;
        GLenum			whichBuffer; 
        GLboolean		isDoubleBuffer, isStereo;
        char*                   buffername = NULL;
        
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	//cap the numbers of inputs and outputs
	PsychErrorExit(PsychCapNumInputArgs(3));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(1));  //The maximum number of outputs
        	
	// Get windowRecord for this window:
        PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);

        // Enable GL-Context of (associated) onscreen window:
        PsychSetGLContext(windowRecord);

        glGetBooleanv(GL_DOUBLEBUFFER, &isDoubleBuffer);
	glGetBooleanv(GL_STEREO, &isStereo);
        
	// Retrieve optional read rectangle:
        PsychGetRectFromWindowRecord(windowRect, windowRecord);
	if(!PsychCopyInRectArg(2, FALSE, sampleRect)) memcpy(sampleRect, windowRect, sizeof(PsychRectType));
	if (IsPsychRectEmpty(sampleRect)) return(PsychError_none);

	// Assign read buffer:
	if(PsychIsOnscreenWindow(windowRecord)) {
	  // Onscreen window: We read from the front- or front-left buffer by default.
	  // This works on single-buffered and double buffered contexts in a consistent fashion:
	  whichBuffer=GL_FRONT;

	  // Copy in optional override buffer name:
	  PsychAllocInCharArg(3, FALSE, &buffername);
	  
	  // Override buffer name provided?
	  if (buffername) {
	    // Which one is it?

	    // "frontBuffer" is always a valid choice:
	    if (strcmp(buffername, "frontBuffer")==0) whichBuffer = GL_FRONT;
	    // Allow selection of left- or right front stereo buffer in stereo mode:
	    if (strcmp(buffername, "frontLeftBuffer")==0 && isStereo) whichBuffer = GL_FRONT_LEFT;
	    if (strcmp(buffername, "frontRightBuffer")==0 && isStereo) whichBuffer = GL_FRONT_RIGHT;
	    // Allow selection of backbuffer in double-buffered mode:
	    if (strcmp(buffername, "backBuffer")==0 && isDoubleBuffer) whichBuffer = GL_BACK;
	    // Allow selection of left- or right back stereo buffer in stereo mode:
	    if (strcmp(buffername, "backLeftBuffer")==0 && isStereo && isDoubleBuffer) whichBuffer = GL_BACK_LEFT;
	    if (strcmp(buffername, "backRightBuffer")==0 && isStereo && isDoubleBuffer) whichBuffer = GL_BACK_RIGHT;
	    // Allow AUX buffer access for debug purposes:
	    if (strcmp(buffername, "aux0Buffer")==0) whichBuffer = GL_AUX0;
	    if (strcmp(buffername, "aux1Buffer")==0) whichBuffer = GL_AUX1;
	    if (strcmp(buffername, "aux2Buffer")==0) whichBuffer = GL_AUX2;
	    if (strcmp(buffername, "aux3Buffer")==0) whichBuffer = GL_AUX3;
	  }
	}
	else {
	  // Offscreen window or texture: They only have one buffer, which is the
	  // backbuffer in double-buffered mode and the frontbuffer in single buffered mode:
	  whichBuffer=(isDoubleBuffer) ? GL_BACK : GL_FRONT;
	}

	// Enable this windowRecords framebuffer as current drawingtarget. This should
	// also allow us to "GetImage" from Offscreen windows:
	if (windowRecord->imagingMode & kPsychNeedFastBackingStore) {
		// Special case: Imaging pipeline active - We need to activate system framebuffer
		// so we really read the content of the framebuffer and not of some FBO:
		PsychSetDrawingTarget(NULL);
	}
	else {
		// Normal case: No FBO based imaging - Select drawing target as usual:
		PsychSetDrawingTarget(windowRecord);
	}
	
	// Select requested read buffer:
	glReadBuffer(whichBuffer);
	
	sampleRectWidth=PsychGetWidthFromRect(sampleRect);
	sampleRectHeight=PsychGetHeightFromRect(sampleRect);
	
	//Note this will only work correctly for 8-bit pixels.  
	if(PsychAllocOutUnsignedByteMatArg(1, TRUE, sampleRectHeight, sampleRectWidth, 3, &returnArrayBase)){
	  redPlane= PsychMallocTemp(3 * sizeof(GL_UNSIGNED_BYTE) * sampleRectWidth * sampleRectHeight);
	  planeSize=sampleRectWidth * sampleRectHeight;
	  greenPlane= redPlane + planeSize;
	  bluePlane= redPlane + 2 * planeSize;
	  glPixelStorei(GL_PACK_ALIGNMENT,1);
	  invertedY=windowRect[kPsychBottom]-sampleRect[kPsychBottom];
	  glReadPixels(sampleRect[kPsychLeft],invertedY, 	sampleRectWidth, sampleRectHeight, GL_RED, GL_UNSIGNED_BYTE, redPlane); 
	  glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_GREEN, GL_UNSIGNED_BYTE, greenPlane);
	  glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_BLUE, GL_UNSIGNED_BYTE, bluePlane);
	  //in one pass transpose and flip what we read with glReadPixels before returning.  
	  //-glReadPixels insists on filling up memory in sequence by reading the screen row-wise whearas Matlab reads up memory into columns.
	  //-the Psychtoolbox screen as setup by gluOrtho puts 0,0 at the top left of the window but glReadPixels always believes that it's at the bottom left.     
	  for(ix=0;ix<sampleRectWidth;ix++){
	    for(iy=0;iy<sampleRectHeight;iy++){
	      redReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth, 3, iy, ix, 0);
	      greenReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth, 3, iy, ix, 1);
	      blueReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth, 3, iy, ix, 2);
	      returnArrayBase[redReturnIndex]=redPlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];  
	      returnArrayBase[greenReturnIndex]=greenPlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];
	      returnArrayBase[blueReturnIndex]=bluePlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];
	    }
	  }
	}
	
	return(PsychError_none);
}

