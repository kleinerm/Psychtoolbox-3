/*
	SCREENFillRect.c		
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
	
		Only OS X for now.
    

	HISTORY:

		09/12/02	awi		Created.  
		10/12/04	awi		In useString: changed "SCREEN" to "Screen", and moved commas to inside [].
		2/25/05		awi		Relocated PsychSetGLContext() to outside condtional, it only executed for small rects.
							glClearColor() now sets variable alpha, not static at 1.0 (255). 
							Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
 
 
	TO DO:
  
		FillRect should detect when it is called without a rect argument and use glClearColor to clear the 
		window to the specified color because  gl implementations may optinally accellerate that. 
  
		Break out the actual rendering commands into a seperate function because OpenWindow and OpenOffscreenWindow
		need to call FillRect to initialize the window to the correct color.  
  
*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('FillRect', windowPtr [,color] [,rect] )";                                          
//                                            1           2        3      
static char synopsisString[] = 
	"Fill \"rect\". \"color\" is the clut index (scalar or [r g b] triplet) that you want "
	"to poke into each pixel;  default produces white with the standard CLUT for this "
	"window's pixelSize. Default \"rect\" is entire window, so you can use this "
	"function to clear the window.";
static char seeAlsoString[] = "FrameRect";	

PsychError SCREENFillRect(void)  
{
	
	PsychColorType					color;
	PsychRectType					rect;
	PsychWindowRecordType			*windowRecord;
	int								depthValue, whiteValue, colorPlaneSize, numColorPlanes;
	boolean							isArgThere, isScreenRect;
	GLdouble						dVals[4]; 
    
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(4));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs

	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(1, kPsychArgRequired, &windowRecord);
	
	//Get the depth from the window, we need this to interpret the color argument.
	depthValue=PsychGetWindowDepthValueFromWindowRecord(windowRecord);
	numColorPlanes=PsychGetNumPlanesFromDepthValue(depthValue);
	colorPlaneSize=PsychGetColorSizeFromDepthValue(depthValue);

	//Get the color argument or use the default, then coerce to the form determened by the window depth.  
	isArgThere=PsychCopyInColorArg(2, FALSE, &color);
	if(!isArgThere){
		whiteValue=PsychGetWhiteValueFromDepthValue(depthValue);
		PsychLoadColorStruct(&color, kPsychIndexColor, whiteValue ); //index mode will coerce to any other.
	}
 	PsychCoerceColorModeFromSizes(numColorPlanes, colorPlaneSize, &color);
        
	//get the rect and draw it
	isScreenRect=FALSE;
	isArgThere=PsychCopyInRectArg(kPsychUseDefaultArgPosition, FALSE, rect);	
	isScreenRect= !isArgThere || isArgThere && PsychMatchRect(rect, windowRecord->rect);
	PsychSetGLContext(windowRecord);
   // Enable this windowRecords framebuffer as current drawingtarget:
   PsychSetDrawingTarget(windowRecord);

	PsychUpdateAlphaBlendingFactorLazily(windowRecord);

	if(isScreenRect && PsychIsOnscreenWindow(windowRecord)){
		// Fullscreen rect fill which in GL is a special case which may be accelerated.
      // We only use this fast-path on real onscreen windows, not on textures or
      // offscreen windows.
		dVals[3]=1.0;
		PsychConvertColorAndDepthToDoubleVector(&color, depthValue, dVals);
		glClearColor(dVals[0], dVals[1], dVals[2], dVals[3]);
		glClear(GL_COLOR_BUFFER_BIT);
	}else{
		// Subregion fill or fullscreen fill into offscreen window or texture: Draw a colored rect.
		PsychSetGLColor(&color, depthValue);

		if (isScreenRect) {
			// Fullscreen fill of a non-onscreen window:
			PsychGLRect(windowRecord->rect);
		} else {
			// Partial fill: Draw provided rect:
			PsychGLRect(rect);
		}
	}
       
   // Mark end of drawing op. This is needed for single buffered drawing:
   PsychFlushGL(windowRecord);

 	//All psychfunctions require this.
	return(PsychError_none);
}




