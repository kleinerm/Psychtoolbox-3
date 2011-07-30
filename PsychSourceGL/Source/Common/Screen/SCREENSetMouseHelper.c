/*
	SCREENSetMouseHelper.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
		mario.kleiner@tuebingen.mpg.de  mk

	PLATFORMS:	
	
		All.
    
	HISTORY:

		12/14/02	awi		Created.
		10/12/04	awi		Changed "SCREEN" to "Screen" in useString.
		11/16/04	awi		Added "Helper" suffix. 
		11/18/04	awi		Renamed to SetMousHelper from PositionCursorHelper

	DESCRIPTION:
  
		Position the mouse pointer

	TO DO:
  
		-The hidecursor.m script should be modified to invoke hidecursor through screen only on OS X.  On other platforms it is a 
		separate mex file.  
  
		-If we store a pointer to the window and screen lists in shared memory (accessible between mex files) then 
		we could implement this a a seperate mex file.  Seems more natural to do it from SCREEN though.  Let's wait and
		see what Windows and Linux are like before changeing it to a separate mex file.  
  
		-Since all that we pass to the underlying CoreGraphics call is a screen ID, maybe we don't in face need to 
		have opened an onscreen window to more the mouse pointer.  Test that.   We will have to have enumberated the 
		displays though.  
  

*/


#include "Screen.h"


// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('SetMouseHelper', windowPntrOrScreenNumber, x, y [, mouseIndex]);";
//                                                  1                         2  3  4
static char synopsisString[] = 
	"This is a helper function called by SetMouse.  Do not call Screen(\'SetMouseHelper\'), use "
	"SetMouse instead.\n"

	"Move the mouse pointer";
static char seeAlsoString[] = "";
	 
PsychError SCREENSetMouseHelper(void) 
{
	int	screenNumber;
	int 	xPos, yPos;
	int     deviceIdx = -1;

	//all subfunctions should have these two lines.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs(4));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
        
	PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
	PsychCopyInIntegerArg(2,TRUE, &xPos);
	PsychCopyInIntegerArg(3,TRUE, &yPos);
	PsychCopyInIntegerArg(4,FALSE, &deviceIdx);

	PsychPositionCursor(screenNumber, xPos, yPos, deviceIdx);

	return(PsychError_none);
}
