/*
	SCREENFillOval.c		
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:
		
		Only OS X for now.
    

	HISTORY:
	
		mm/dd/yy
	
		10/10/03	awi		Created.  Based on SCREENFillRect and SCREENgluDisk.
		10/12/03	awi		Changed help string for new argument order.
		10/12/04	awi		In useString: changed "SCREEN" to "Screen", and moved commas to inside [].
		1/15/05		awi		Removed GL_BLEND setting a MK's suggestion.  
		2/25/05		awi		Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
		

	TO DO:
  

*/


#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('FillOval', windowPtr [,color] [,rect]);";
static char synopsisString[] = 
        "Fills an ellipse with the given color, inscribed within \"rect\".\"color\" is the "
        "clut index (scalar or [r g b] triplet) that you want to poke into each pixel; "
        "default produces white with the standard CLUT for this window's pixelSize. "
        "Default rect is whole window. ";
static char seeAlsoString[] = "FrameOval";	

PsychError SCREENFillOval(void)  
{
	
	PsychColorType			color;
	PsychRectType			rect;
	double				numSlices, radius, xScale, yScale, xTranslate, yTranslate, rectY, rectX;
	PsychWindowRecordType	        *windowRecord;
	int				depthValue, whiteValue;
	boolean				isArgThere;
	GLUquadricObj			*diskQuadric;
    
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);}
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(5));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs

	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
        
	//Get the depth from the window, we need this to interpret the color argument.
	depthValue=PsychGetWindowDepthValueFromWindowRecord(windowRecord);
	

	//Get the color argument or use the default, then coerce to the form determened by the window depth.  
	isArgThere=PsychCopyInColorArg(kPsychUseDefaultArgPosition, FALSE, &color);
	if(!isArgThere){
		whiteValue=PsychGetWhiteValueFromWindow(windowRecord);
		PsychLoadColorStruct(&color, kPsychIndexColor, whiteValue ); //index mode will coerce to any other.
	}
 	PsychCoerceColorMode( &color);
        

	//get the rect value
	isArgThere=PsychCopyInRectArg(kPsychUseDefaultArgPosition, FALSE, rect);
	if(!isArgThere) PsychCopyRect(rect, windowRecord->rect);
	if (IsPsychRectEmpty(rect)) return(PsychError_none);


	//The glu disk object location and size with a  center point and a radius,   
	//whereas FillOval accepts a bounding rect.   Converting from one set of parameters
	//to the other we should careful what we do for rects size of even number of pixels in length.
	PsychGetCenterFromRectAbsolute(rect, &xTranslate, &yTranslate);
	rectY=PsychGetHeightFromRect(rect);
	rectX=PsychGetWidthFromRect(rect);
	if(rectX == rectY){
		xScale=1; 
		yScale=1;
		radius=rectX/2;
	}else if(rectX > rectY){ 
		xScale=1;
		yScale=rectY/rectX;
		radius=rectX/2;
	}else if(rectY > rectX){
		yScale=1;
		xScale=rectX/rectY;
		radius=rectY/2;
	}
	numSlices=3.14159265358979323846 * 2 * radius;
        
        
	//Set the context & color
	PsychSetGLContext(windowRecord);
        // Enable this windowRecords framebuffer as current drawingtarget:
        PsychSetDrawingTarget(windowRecord);

	PsychUpdateAlphaBlendingFactorLazily(windowRecord);
	PsychSetGLColor(&color,  windowRecord);
        
	//Draw  
	glPushMatrix();
	glTranslated(xTranslate,yTranslate,0);
	glScaled(xScale, yScale, 1);
	diskQuadric=gluNewQuadric();
	gluDisk(diskQuadric, 0, radius, numSlices, 1);
	gluDeleteQuadric(diskQuadric);
	glPopMatrix();
       
        // Mark end of drawing op. This is needed for single buffered drawing:
        PsychFlushGL(windowRecord);

 	//All psychfunctions require this.
	return(PsychError_none);
}





