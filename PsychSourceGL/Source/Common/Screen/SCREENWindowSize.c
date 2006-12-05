/*
  SCREENWindowSize.c		
  
  AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:
  
		Only OS X for now.    

  HISTORY:
  
		2/26/05  awi		Created.  Inspired by Frans Cornelissen's script "WindowSize".   
 
  
  TO DO:
  

*/


#include "Screen.h"



// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[width, height]=Screen('WindowSize', windowPointerOrScreenNumber);";
//                                             1
static char synopsisString[] = 
	"Return the width and height of a window or screen in units of pixels.";
static char seeAlsoString[] = "Screen('Rect')";	

PsychError SCREENWindowSize(void)  
{
	
	PsychWindowRecordType *windowRecord;
	int screenNumber;
	PsychRectType rect;
	double	rectWidth, rectHeight;
    
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(1));		//The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1));	//Insist that the argument be present.   
	PsychErrorExit(PsychCapNumOutputArgs(2));		//The maximum number of outputs

	if(PsychIsScreenNumberArg(1)){
		PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
		PsychGetScreenRect(screenNumber, rect);
		rectWidth=PsychGetWidthFromRect(rect);
		rectHeight=PsychGetHeightFromRect(rect);
		PsychCopyOutDoubleArg(1, kPsychArgOptional, rectWidth);
		PsychCopyOutDoubleArg(2, kPsychArgOptional, rectHeight);
	}else if(PsychIsWindowIndexArg(1)){
		PsychAllocInWindowRecordArg(1, TRUE, &windowRecord);
		rectWidth=PsychGetWidthFromRect(windowRecord->rect);
		rectHeight=PsychGetHeightFromRect(windowRecord->rect);

		if (windowRecord->stereomode==kPsychFreeFusionStereo || windowRecord->stereomode==kPsychFreeCrossFusionStereo) {
			// Special case for stereo: Only half the real window width:
			rectWidth = rectWidth / 2;
		}

		PsychCopyOutDoubleArg(1, kPsychArgOptional, rectWidth);
		PsychCopyOutDoubleArg(2, kPsychArgOptional, rectHeight);
	}else
		PsychErrorExitMsg(PsychError_user, "Argument was recognized as neither a window index nor a screen pointer");

	return(PsychError_none);
}

PsychError SCREENDisplaySize(void)  
{
    static char useString[] = "[width, height]=Screen('DisplaySize', ScreenNumber);";
    static char synopsisString[] = 
	"Return the physical width and height of the output display device associated with 'ScreenNumber'. "
        "The size is returned in units of millimeters as reported by the display device itself to the OS. "
        "On MacOS-X, if Extended Display Identification Data (EDID) for the display device is not "
        "available, the size is estimated by OS-X based on the device width and height in pixels, with an "
        "assumed resolution of 2.835 pixels/mm or 72 DPI, a reasonable guess for displays predating "
        "EDID support. On M$-Windows and GNU/Linux, the behaviour in case of missing EDID data is "
        "unknown. This function returns a width and height of zero if physical display size can't be "
        "queried from the operating system. Please handle the returned information with great caution. "
        "It is unclear how accurate the EDID data for typical monitors or video beamers really is. This "
        "information could be pretty unreliable and therefore misleading!";
    static char seeAlsoString[] = "";	    
    int screenNumber, Width, Height;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(1));		//The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1));	//Insist that the argument be present.   
    PsychErrorExit(PsychCapNumOutputArgs(2));		//The maximum number of outputs
    
    // Retrieve screen number:
    PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
    // Query physical size of this screen in millimeters:
    PsychGetDisplaySize(screenNumber, &Width, &Height);
    // Return it:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) Width);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) Height);
    
    return(PsychError_none);
}


