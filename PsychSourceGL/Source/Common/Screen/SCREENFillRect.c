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
	int								whiteValue;
	boolean							isArgThere, isScreenRect;
	GLdouble						dVals[4]; 
    double							*xy, *colors;
	unsigned char					*bytecolors;
	int								numRects, i, nc, mc, nrsize;

	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(4));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs

	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(1, kPsychArgRequired, &windowRecord);
	
	// Query, allocate and copy in all vectors...
	numRects = 4;
	nrsize = 0;
	colors = NULL;
	bytecolors = NULL;
	// The negative position -3 means: xy coords are expected at position 3, but they are optional.
	// NULL means - don't want a size's vector.
	PsychPrepareRenderBatch(windowRecord, -3, &numRects, &xy, 2, &nc, &mc, &colors, &bytecolors, 0, &nrsize, NULL);
	isScreenRect=FALSE;
	
	// Only up to one rect provided?
	if (numRects <= 1) {
		// Get the rect and draw it
		isArgThere=PsychCopyInRectArg(kPsychUseDefaultArgPosition, FALSE, rect);	
		isScreenRect= !isArgThere || isArgThere && PsychMatchRect(rect, windowRecord->rect);
		if (isArgThere && IsPsychRectEmpty(rect)) return(PsychError_none);
	}
	
	if(isScreenRect && PsychIsOnscreenWindow(windowRecord) && 
	   (windowRecord->stereomode < kPsychAnaglyphRGStereo || windowRecord->stereomode > kPsychAnaglyphBRStereo || windowRecord->imagingMode > 0)){
		// Fullscreen rect fill which in GL is a special case which may be accelerated.
		// We only use this fast-path on real onscreen windows, not on textures or
		// offscreen windows.
		
		//Get the color argument or use the default, then coerce to the form determened by the window depth.  
		isArgThere=PsychCopyInColorArg(2, FALSE, &color);
		if(!isArgThere){
			whiteValue=PsychGetWhiteValueFromWindow(windowRecord);
			PsychLoadColorStruct(&color, kPsychIndexColor, whiteValue ); //index mode will coerce to any other.
		}
		PsychCoerceColorMode( &color);
				
		dVals[3]=1.0;
		PsychConvertColorToDoubleVector(&color, windowRecord, dVals);
		glClearColor(dVals[0], dVals[1], dVals[2], dVals[3]);
		glClear(GL_COLOR_BUFFER_BIT);
	}else{
	  // Subregion fill or fullscreen fill into offscreen window or texture: Draw a colored rect.

	  // At this point if we have a single color value, it is already set by PsychPrerpareRenderBatch,
	  // so we can skip this: PsychSetGLColor(&color, windowRecord);
	  
	  // Fullscreen or partial fill?
	  if (isScreenRect) {
	    // Fullscreen fill of a non-onscreen window:
	    PsychGLRect(windowRecord->rect);
	  } else {
	    // Partial fill: Draw provided rects:
		if (numRects>1) {
			// Multiple rects provided: Draw the whole batch:
			for (i=0; i<numRects; i++) {
				// Per rect color provided?
				if (nc>1) {
					// Yes. Set color for this specific rect:
					if (mc==3) {
						if (colors) {
							// RGB double:
							glColor3dv(&(colors[i*3]));
						}
						else {
							// RGB uint8:
							glColor3ubv(&(bytecolors[i*3]));
						}
					}
					else {
						if (colors) {
							// RGBA double:
							glColor4dv(&(colors[i*4]));
						}
						else {
							// RGBA uint8:
							glColor4ubv(&(bytecolors[i*4]));
						}					
					}
				}
				
				// Submit rect for drawing:
				PsychGLRect((PsychRectType*) &(xy[i*4]));
			}
		}
		else {
			// Single partial screen rect provided: Draw it.
			PsychGLRect(rect);
		}
	  }
	}
	
	// Mark end of drawing op. This is needed for single buffered drawing:
	PsychFlushGL(windowRecord);
	
 	//All psychfunctions require this.
	return(PsychError_none);
}
