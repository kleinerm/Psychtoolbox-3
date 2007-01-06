/*
	SCREENFrameRect.c		

	AUTHORS:

		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:	

		Only OS X for now.


	HISTORY:

		07/23/04	awi		Created.
		10/12/04	awi		In useString: moved commas to inside [].
		2/25/05		awi		Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
		  

	TO DO:

*/


#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('FrameRect', windowPtr [,color] [,rect] [,penWidth]);";
//                                             1           2        3       4          
static char synopsisString[] = 
	"Draw the outline of a rectangle. \"color\" is the clut index (scalar or [r g b a] "
    "vector) that you want to poke into each pixel; default produces black "
    "Default \"rect\" is entire window. "
    "Default pen size is 1. ";
	
static char seeAlsoString[] = "FillRect";	

PsychError SCREENFrameRect(void)  
{
	
	PsychColorType					color;
	PsychRectType					rect;
	PsychWindowRecordType			*windowRecord;
	int								depthValue, whiteValue;
	boolean							isArgThere;
	double							penSize;
    
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
	

	//Get the color argument or use the default, then coerce to the form determened by the window depth.  
	isArgThere=PsychCopyInColorArg(2, FALSE, &color);
	if(!isArgThere){
		whiteValue=PsychGetWhiteValueFromWindow(windowRecord);
		PsychLoadColorStruct(&color, kPsychIndexColor, whiteValue ); //index mode will coerce to any other.
	}
 	PsychCoerceColorMode( &color);
        
	//get the rect
	PsychCopyRect(rect, windowRecord->rect);
	PsychCopyInRectArg(3, FALSE, rect);
	if (IsPsychRectEmpty(rect)) return(PsychError_none);
	
	//get the pen size
	penSize=1;
	PsychCopyInDoubleArg(4, kPsychArgOptional, &penSize);
	
	//draw the rect
	PsychSetGLContext(windowRecord);
        // Enable this windowRecords framebuffer as current drawingtarget:
        PsychSetDrawingTarget(windowRecord);

	glLineWidth((GLfloat)penSize);
	PsychUpdateAlphaBlendingFactorLazily(windowRecord);
	PsychSetGLColor(&color, windowRecord);
	glBegin(GL_LINE_LOOP);
		glVertex2d(rect[kPsychLeft], rect[kPsychBottom]);
		glVertex2d(rect[kPsychLeft], rect[kPsychTop]);
		glVertex2d(rect[kPsychRight], rect[kPsychTop]);
		glVertex2d(rect[kPsychRight], rect[kPsychBottom]);
	glEnd();

        // Mark end of drawing op. This is needed for single buffered drawing:
        PsychFlushGL(windowRecord);

	return(PsychError_none);
}





