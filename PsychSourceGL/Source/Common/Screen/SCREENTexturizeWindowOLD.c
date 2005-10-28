/*
  SCREENTexturizeWindow.c	
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	All
    

  HISTORY:
  3/13/03  awi		Created.    
 
  DESCRIPTION:
  
	
  NOTES:
  
  
  RESOURCES:

*/


#include "Screen.h"


static char useString[] = "SCREEN('TexturizeWindow', sourceWindowPntr, [targetWindowPntr]);";
static char synopsisString[] = 
	"For the window designated by sourceWindowPntr create a texture which will accelerate copying by CopyWindow to "
	"to a target window.  If no target is specified then the default is that given by the screen number or window "
	"pointer already associated with the window by the window-pointer-or-screen-number argument supplied when that "
	"window was opened.";
	 
static char seeAlsoString[] = "";


PsychError SCREENTexturizeWindow(void) 
{
    PsychWindowRecordType 	*sourceWin, *targetWin;
	Boolean					isThere;
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous or missing arguments
	PsychErrorExit(PsychCapNumInputArgs(2));   	
    PsychErrorExit(PsychRequireNumInputArgs(1)); 	

    
    //Get the window structure for the onscreen and offscreen windows.
	PsychAllocInWindowRecordArg(1, TRUE, &sourceWin);
	isThere=PsychAllocInWindowRecordArg(2, FALSE, &targetWin);

	//each of these commands only does work if neccessary so they can be called invoked overgenerously if in doubt
	if(isThere)
		PsychRetargetWindowToWindow(sourceWin, targetWin);
	PsychUpdateTargetWindowFromTargetDisplay(sourceWin);
	PsychAllocateTexture(sourceWin);
	PsychBindTexture(sourceWin);
	PsychUpdateTexture(sourceWin);
	
    return(PsychError_none);

}



	
	





