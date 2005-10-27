/*
	SCREENOpenOffscreenWindow.c		
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
		
		Only OS X for now.
    

	HISTORY:

		1/18/02		awi		Created.  Copied the Synopsis string from old version of psychtoolbox.
		2/18/03		awi		Set the screen number for all offscreen windows to -1.   
							What use does the screen number serve for offscren windows ?
							¥Because we use glDrawPixels and glReadPixels to move pixels to and from surfaces, and those handle 
							alignment on the surface, knowing the parent window is not necessary for the purpose of matching 
							alignments in copy operations. 
							¥The bit depths do need to match for CopyWindow, and we could use the parent window as an indicator of 
							matching depth.  However, if we want to match depths, it is better to compare depths than to use 
							the parent window number as a proxy for depth; It is more transparent and more general because it handles copying 			
							between two onscreen windows of equal depth but which do not share the same parent window.
		10/12/04	awi		In useString: moved commas to inside [].
		2/15/05		awi		Commented out glEnable(GL_BLEND) 
						 

     
	TO DO:
  
		Find out if the window background color is supposed to be always white,  or the color of the parent window.   
  

*/


#include "Screen.h"


// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] =  "[windowPtr,rect]=Screen('OpenOffscreenWindow',windowPtrOrScreenNumber [,color] [,rect] [,pixelSize]);";
//                                                                        1                         2        3       4                  
static char synopsisString[] =
    "Open an offscreen window, which Apple calls a GWorld. This is simply an area in "
    "memory that is treated as a window. Offscreen windows are very useful for "
    "creating images for subsequent display, because copying between windows is very "
    "fast (e.g. 36 MB/s on a PowerMac 7500/100, or 180 MB/s on a G4/800). You may "
    "specify a screen (any windowPtr or a screenNumber>=0) or no screen "
    "(screenNumber=-1). If you do designate a screen then the offscreen window is "
    "aligned (in memory) for the fastest possible copying to the specified rect of "
    "that screen. You may specify no screen (screenNumber=-1), but any real screen "
    "must already have an open Screen window when you call OpenOffscreenWindow. "
    "\"color\" is the clut index (scalar or [r g b] triplet) that you want to poke into "
    "each pixel; default is white. \"rect\" specifies the size of the offscreen window "
    "and what part of the screen the offscreen window is aligned with. If supplied, "
    "\"rect\" must contain at least one pixel. If a windowPtr is supplied, then \"rect\" "
    "is in the window's coordinates (origin at upper left), and defaults to the whole "
    "window. If a screenNumber is supplied then \"rect\" is in screen coordinates "
    "(origin at upper left), and defaults to the whole screen. If a screenNumber of "
    "-1 is supplied, then \"rect\" defaults to the size of the main screen. (In all "
    "cases, subsequent references to this new offscreen window will use its "
    "coordinates: origin at its upper left.) \"pixelSize\" sets the depth (in bits) of "
    "each pixel. If you specify a screen (screenNumber>=0) and a pixelSize>0 then "
    "they must be consistent. If you specify no screen (screenNumber=-1) then the "
    "default pixelSize is 8, but you can specify any legal depth: 1, 2, 4, 8, 16, 32. "
    "A pixelSize of 0 or [] is replaced by the default. "

    "NOTE: Screen's windows are known only to Screen and must be closed by it, eg "
    "Screen(w,'Close'). Matlab knows nothing about Screen's windows, so the Matlab "
    "CLOSE command won't work on Screen's windows. ";
static char seeAlsoString[] = "OpenWindow";
	

PsychError SCREENOpenOffscreenWindow(void) 
{
    int							screenNumber, depth, targetScreenNumber;
    PsychRectType				rect;
    PsychColorType				color;
    PsychColorModeType  		mode; 
    boolean						didWindowOpen;
    PsychWindowRecordType		*exampleWindowRecord, *windowRecord, *targetWindow;
	Boolean						wasColorSupplied;
	


	//all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //cap the number of inputs
    PsychErrorExit(PsychCapNumInputArgs(5));   //The maximum number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(2));  //The maximum number of outputs
    
    //1-User supplies a window ptr 2-User supplies a screen number 3-User supplies rect and pixel size
    if(PsychIsWindowIndexArg(1)){
        PsychAllocInWindowRecordArg(1, TRUE, &exampleWindowRecord);
        PsychCopyRect(rect, exampleWindowRecord->rect);
        depth=exampleWindowRecord->depth;
		targetScreenNumber=exampleWindowRecord->screenNumber;
//		if(PsychIsOnscreenWindow(exampleWindowRecord))
//			targetWindow=exampleWindowRecord;
//		else
//			targetWindow=exampleWindowRecord->targetWindow;
    } else if(PsychIsScreenNumberArg(1)){
        PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
        PsychGetScreenRect(screenNumber, rect);
        depth=PsychGetScreenDepthValue(screenNumber);
		targetScreenNumber=screenNumber;
		targetWindow=NULL;
    } else if(PsychIsUnaffiliatedScreenNumberArg(1)){  //that means -1 or maybe also NaN if we add that option.  
        //we require both the rect and the depth because there no example from which to inherit those examples is provided.
        PsychCopyInRectArg(3, TRUE, rect);
        PsychCopyInDepthValueArg(4, TRUE, &depth);
		targetScreenNumber=kPsychUnaffiliatedWindow;
		targetWindow=NULL;
    } else
		PsychErrorExit(PsychError_invalidNumdex);
		
    
   
    //Depth and rect argument supplied as arguments override those inherited from reference screen or window.
	//not that PsychCopyIn* prefix means that value will not be overwritten if the arguments are not present.
    PsychCopyInRectArg(3,FALSE, rect);
    PsychCopyInDepthValueArg(4,FALSE, &depth); 
        
    //find the color for the window background.  
    wasColorSupplied=PsychCopyInColorArg(kPsychUseDefaultArgPosition, FALSE, &color); //get from user
    if(!wasColorSupplied)
		PsychLoadColorStruct(&color, kPsychIndexColor, PsychGetWhiteValueFromDepthValue(depth)); //or use the default
    mode=PsychGetColorModeFromDepthValue(depth);  
    PsychCoerceColorMode(mode, &color);
    
    //open the window
    didWindowOpen=PsychOpenOffscreenWindow(rect, depth, &windowRecord);
    if(!didWindowOpen){
        PsychErrorExitMsg(PsychError_internal, "Failed to open offscreen window.  There could be insufficient memory");
    }

	//fill in the window record.
//	windowRecord->targetScreenNumber=targetScreenNumber;
//	windowRecord->targetWindow=targetWindow;
	
//	PsychAllocateShadow(windowRecord);

    
    //TO DO: Fill the window with the background color.  Actually, this should be passed to OpenWindow and be part of the 
    //window structure because of GLClearColor.
	
	//set the alpha blending rule   
	PsychSetGLContext(windowRecord); 
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//			sFactor		   dFactor

    
    //Return the window index and the rect argument.
    PsychCopyOutDoubleArg(1, FALSE, windowRecord->windowIndex);
    PsychCopyOutRectArg(2, FALSE, rect);
	
	
    return(PsychError_none);    
}
