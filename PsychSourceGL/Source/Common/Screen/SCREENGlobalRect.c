/*
  SCREENGlobalRect.c		
  
  AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:
  
		Only OS X for now.    

  HISTORY:
  
		10/16/04  awi		Created.   
 
  
  TO DO:
  
		If passed a window pointer,  this function returns the global rect for a window's parent screen, not the rect for the window
		itself.  That's ok, because in the OS X Psychtoolbox we only allow fullscren windows, so the window rect is always the same
		as the  window's parent screen rect.  If we ever remove the fullscreen window  requirement, then this function should be
		modified to return the window's rect when passed a window pointer.  
  

*/


#include "Screen.h"



// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "rect=Screen('GlobalRect', windowPointerOrScreenNumber);";
//                                                   1           
static char synopsisString[] = 
	"Get global rect of window or screen.";
static char seeAlsoString[] = "";	

PsychError SCREENGlobalRect(void)  
{
	
	int						screenNumber;
	PsychRectType			rect; 
    
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(1));			
	PsychErrorExit(PsychRequireNumInputArgs(1));		 
	PsychErrorExit(PsychCapNumOutputArgs(1));			

	PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
	PsychGetGlobalScreenRect(screenNumber, rect);
	PsychCopyOutRectArg(1, FALSE, rect);

	return(PsychError_none);
}





