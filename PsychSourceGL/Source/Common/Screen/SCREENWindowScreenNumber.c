/*
	Psychtoolbox3/Source/Common/SCREENWindowScreenNumber	
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	

		This file should build on any platform. 

	HISTORY:
		1/24/03  awi		Created. 
 
	DESCRIPTION:
  
		For onscreen window: Return the number of the screen on which it lies. 
		For offscreen window: Return the number of the ancestral screen.  If none, return -1.    
  
	TO DO:
	
		When we re-write to represent the pixel format with a single constant instead of structure change the test for matching 
		pixel depth to a test for equivalent color resolution.  Note that the pixel format need not be the same, only 
		the same resolution, for example, kPsych_R8_G8_B8_A8 is compatible with kPsych_A8_R8_G8_B8 but not kPsych_R10_G10_B10_A2.
  
*/
    

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "screenNumber=Screen('WindowScreenNumber', windowPtr);";
//                                                                   1											
static char synopsisString[] = 
	"Get screenNumber, the number of the screen on which the window appears.  If the window is an offscreen window then this value is -1. ";
	
static char seeAlsoString[] = "";
	 
PsychError SCREENWindowScreenNumber(void) 
{
    PsychWindowRecordType	*windowRecord;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychAllocInWindowRecordArg(1, TRUE, &windowRecord);
    PsychCopyOutDoubleArg(1, FALSE, windowRecord->screenNumber);
    
    return(PsychError_none);	
}


