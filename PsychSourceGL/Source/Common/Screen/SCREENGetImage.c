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
static char useString[] =  "imageArray=Screen('GetImage', windowPtr [,rect] [,bufferName] [,floatprecision=0] [,nrchannels=3])";
//                                                        1           2       3				4				   5
static char synopsisString[] =
"Slowly copy an image from a window or texture to Matlab, by default returning a Matlab uint8 array. "
"The returned imageArray by default has three layers, i.e. it's an RGB image. "
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
"otherwise your script will crash, this is mostly meant for internal debugging of PTB. "
"If the imaging pipeline is enabled you can also return the content of the unprocessed "
"backbuffer, ie. before processing by the pipeline, by requesting 'drawBuffer'. \n"
"\"floatprecision\" If you set this optional flag to 1, the image data will be returned "
"as a double precision matrix instead of a uint8 matrix. Please note that normal image "
"data will be returned in the normalized range 0.0 to 1.0 instead of 0 - 255. Floating "
"point readback is only beneficial when reading back floating point precision textures, "
"offscreen windows or the framebuffer when the imaging pipeline is active and HDR mode "
"is selected (ie. more than 8bpc framebuffer).\n"
"\"nrchannels\" Number of color channels to return. by default, 3 channels (RGB) are "
"returned. Specify 1 for Red/Luminance only, 2 for Red+Green or Luminance+Alpha, 3 for "
"RGB and 4 for RGBA. ";

static char seeAlsoString[] = "PutImage";
	

PsychError SCREENGetImage(void) 
{
	PsychRectType   windowRect,sampleRect;
	int 			nrchannels, ix, iy, sampleRectWidth, sampleRectHeight, invertedY, redReturnIndex, greenReturnIndex, blueReturnIndex, alphaReturnIndex, planeSize;
	ubyte 			*returnArrayBase, *redPlane, *greenPlane, *bluePlane, *alphaPlane;
	float 			*dredPlane, *dgreenPlane, *dbluePlane, *dalphaPlane;
	double 			*returnArrayBaseDouble;
	PsychWindowRecordType	*windowRecord;
	GLenum			whichBuffer; 
	GLboolean		isDoubleBuffer, isStereo;
	char*           buffername = NULL;
	boolean			floatprecision = FALSE;
	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//cap the numbers of inputs and outputs
	PsychErrorExit(PsychCapNumInputArgs(5));   //The maximum number of inputs
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
	if ((windowRecord->imagingMode & kPsychNeedFastBackingStore) || (windowRecord->imagingMode & kPsychNeedFastOffscreenWindows)) {
		// Special case: Imaging pipeline active - We need to activate system framebuffer
		// so we really read the content of the framebuffer and not of some FBO:
		if (PsychIsOnscreenWindow(windowRecord)) {
			// It's an onscreen window:
			if (buffername && (strcmp(buffername, "drawBuffer")==0) && (windowRecord->imagingMode & kPsychNeedFastBackingStore)) {
				// Activate drawBufferFBO:
				PsychSetDrawingTarget(windowRecord);
				whichBuffer = GL_COLOR_ATTACHMENT0_EXT;
			}
			else {
				// Activate system framebuffer:
				PsychSetDrawingTarget(NULL);
			}
		}
		else {
			// Offscreen window or texture: Select drawing target as usual,
			// but set color attachment as read buffer:
			PsychSetDrawingTarget(windowRecord);
			whichBuffer = GL_COLOR_ATTACHMENT0_EXT;
		}
	}
	else {
		// Normal case: No FBO based imaging - Select drawing target as usual:
		PsychSetDrawingTarget(windowRecord);
	}
	
	// Select requested read buffer:
	glReadBuffer(whichBuffer);
	
	// Get optional floatprecision flag: We return data with float-precision if
	// this flag is set. By default we return uint8 data:
	PsychCopyInFlagArg(4, FALSE, &floatprecision);
	
	// Get the optional number of channels flag: By default we return 3 channels,
	// the Red, Green, and blue color channel:
	nrchannels = 3;
	PsychCopyInIntegerArg(5, FALSE, &nrchannels);
	if (nrchannels < 1 || nrchannels > 4) PsychErrorExitMsg(PsychError_user, "Number of requested channels 'nrchannels' must be between 1 and 4!");
	
	sampleRectWidth=PsychGetWidthFromRect(sampleRect);
	sampleRectHeight=PsychGetHeightFromRect(sampleRect);
	
	if (!floatprecision) {
		// Readback of standard 8bpc uint8 pixels:  
		PsychAllocOutUnsignedByteMatArg(1, TRUE, sampleRectHeight, sampleRectWidth, nrchannels, &returnArrayBase);
		redPlane= PsychMallocTemp(nrchannels * sizeof(GL_UNSIGNED_BYTE) * sampleRectWidth * sampleRectHeight);
		planeSize=sampleRectWidth * sampleRectHeight;
		greenPlane= redPlane + planeSize;
		bluePlane= redPlane + 2 * planeSize;
		alphaPlane= redPlane + 3 * planeSize; 
		glPixelStorei(GL_PACK_ALIGNMENT,1);
		invertedY=windowRect[kPsychBottom]-sampleRect[kPsychBottom];
		glReadPixels(sampleRect[kPsychLeft],invertedY, 	sampleRectWidth, sampleRectHeight, GL_RED, GL_UNSIGNED_BYTE, redPlane); 
		if (nrchannels>1) glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_GREEN, GL_UNSIGNED_BYTE, greenPlane);
		if (nrchannels>2) glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_BLUE, GL_UNSIGNED_BYTE, bluePlane);
		if (nrchannels>3) glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_ALPHA, GL_UNSIGNED_BYTE, alphaPlane);
		
		//in one pass transpose and flip what we read with glReadPixels before returning.  
		//-glReadPixels insists on filling up memory in sequence by reading the screen row-wise whearas Matlab reads up memory into columns.
		//-the Psychtoolbox screen as setup by gluOrtho puts 0,0 at the top left of the window but glReadPixels always believes that it's at the bottom left.     
		for(ix=0;ix<sampleRectWidth;ix++){
			for(iy=0;iy<sampleRectHeight;iy++){
				// Compute write-indices for returned data:
				redReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth, nrchannels, iy, ix, 0);
				greenReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth,  nrchannels, iy, ix, 1);
				blueReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth,  nrchannels, iy, ix, 2);
				alphaReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth,  nrchannels, iy, ix, 3);
				
				// Always return RED/LUMINANCE channel:
				returnArrayBase[redReturnIndex]=redPlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];  
				// Other channels on demand:
				if (nrchannels>1) returnArrayBase[greenReturnIndex]=greenPlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];
				if (nrchannels>2) returnArrayBase[blueReturnIndex]=bluePlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];
				if (nrchannels>3) returnArrayBase[alphaReturnIndex]=alphaPlane[ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth];
			}
		}		
	}
	else {
		// Readback of standard 32bpc float pixels into a double matrix:  
		PsychAllocOutDoubleMatArg(1, TRUE, sampleRectHeight, sampleRectWidth, nrchannels, &returnArrayBaseDouble);
		dredPlane= PsychMallocTemp(nrchannels * sizeof(GL_FLOAT) * sampleRectWidth * sampleRectHeight);
		planeSize=sampleRectWidth * sampleRectHeight * sizeof(GL_FLOAT);
		dgreenPlane= redPlane + planeSize;
		dbluePlane= redPlane + 2 * planeSize;
		dalphaPlane= redPlane + 3 * planeSize; 
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		invertedY=windowRect[kPsychBottom]-sampleRect[kPsychBottom];
		if (nrchannels==1) glReadPixels(sampleRect[kPsychLeft],invertedY, 	sampleRectWidth, sampleRectHeight, GL_RED, GL_FLOAT, dredPlane); 
		if (nrchannels==2) glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_LUMINANCE_ALPHA, GL_FLOAT, dredPlane);
		if (nrchannels==3) glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_RGB, GL_FLOAT, dredPlane);
		if (nrchannels==4) glReadPixels(sampleRect[kPsychLeft],invertedY,	sampleRectWidth, sampleRectHeight, GL_RGBA, GL_FLOAT, dredPlane);
		
		//in one pass transpose and flip what we read with glReadPixels before returning.  
		//-glReadPixels insists on filling up memory in sequence by reading the screen row-wise whearas Matlab reads up memory into columns.
		//-the Psychtoolbox screen as setup by gluOrtho puts 0,0 at the top left of the window but glReadPixels always believes that it's at the bottom left.     
		for(ix=0;ix<sampleRectWidth;ix++){
			for(iy=0;iy<sampleRectHeight;iy++){
				// Compute write-indices for returned data:
				redReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth, nrchannels, iy, ix, 0);
				greenReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth,  nrchannels, iy, ix, 1);
				blueReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth,  nrchannels, iy, ix, 2);
				alphaReturnIndex=PsychIndexElementFrom3DArray(sampleRectHeight, sampleRectWidth,  nrchannels, iy, ix, 3);
				
				// Always return RED/LUMINANCE channel:
				returnArrayBaseDouble[redReturnIndex]=dredPlane[(ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth) * nrchannels + 0];  
				// Other channels on demand:
				if (nrchannels>1) returnArrayBaseDouble[greenReturnIndex]=dredPlane[(ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth) * nrchannels + 1];
				if (nrchannels>2) returnArrayBaseDouble[blueReturnIndex]=dredPlane[(ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth) * nrchannels + 2];
				if (nrchannels>3) returnArrayBaseDouble[alphaReturnIndex]=dredPlane[(ix + ((sampleRectHeight-1) - iy ) * sampleRectWidth) * nrchannels + 3];
			}
		}		
	}
	
	return(PsychError_none);
}
