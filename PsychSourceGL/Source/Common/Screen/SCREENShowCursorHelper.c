/*
	SCREENShowCursorHelper.c	
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:
	
		Only OS X for now.
    
	HISTORY:
	
		12/14/02	awi		Created.
		10/12/04	awi		Changed "SCREEN" to "Screen" in useString.  Added ShowCursor to see also.  Fixed inappropriate "HideCursor" in this file.
		11/16/04	awi		Added "Helper" suffix. 
 
    
 
	DESCRIPTION:
  
		Hides the mouse pointer

  
	TO DO:
  
		The showcursor.m script should be modified to invoke hidecursor through screen only on OS X.  Otherwise it is a 
		separate mex file.  
  
		If we store a pointer to the window and screen lists in shared memory (accessible between mex files) then 
		we could implement this a a seperate mex file.  Seems more natural to do it from SCREEN though.  Let's wait and
		see what Windows and Linux are like before changeing it to a separate mex file.  
  
*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('ShowCursorHelper', windowPntr);";
//                                              1
static char synopsisString[] = 
	"This is a helper function called by ShowCursor.  Do not call Screen(\'ShowCursorHelper\'), use "
	"ShowCursor instead.\n"
	"Show the mouse pointer";
static char seeAlsoString[] = "HideCursorHelper";
	 
PsychError SCREENShowCursorHelper(void) 
{
	int	screenNumber;
 
	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs(1));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
        
	PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
	PsychShowCursor(screenNumber);
	return(PsychError_none);	
}


	
	






