/*
	SCREENHideCursorHelper.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
		mario.kleiner@tuebingen.mpg.de  mk

	PLATFORMS:
	
		All.

	HISTORY:

		12/14/02	awi		Created.  
		10/12/04	awi		Changed "SCREEN" to "Screen" in useString.
		11/16/04	awi		Added "Helper" suffix. 
   
	DESCRIPTION:
  
		Hides the mouse pointer

	TO DO:

*/

#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('HideCursorHelper', windowPntr [, mouseIndex]);";
//                                                    1             2
static char synopsisString[] =
	"This is a helper function called by HideCursor.  Do not call Screen(\'HideCursorHelper\'), use "
	"HideCursor instead.\n"
 	"Hides the mouse pointer";
static char seeAlsoString[] = "ShowCursorHelper";

PsychError SCREENHideCursorHelper(void) 
{
	int	screenNumber, mouseIdx;
 
	//all subfunctions should have these two lines.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs(2));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
        
	//get the screen number from the window record or screen number
	PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);

	mouseIdx = -1;
	PsychCopyInIntegerArg(2, FALSE, &mouseIdx);

	PsychHideCursor(screenNumber, mouseIdx);
	return(PsychError_none);
}
