/*
  SCREENGlobalRect.c		
  
  AUTHORS:
  
		Allen.Ingling@nyu.edu			awi 
		mario.kleiner@tuebingen.mpg.de	mk
  
  PLATFORMS:
  
		All.

  HISTORY:
  
		10/16/04	awi		Created.   
		11/14/08	mk		Extended: Now 'GloblRect' really returns the proper absolute window location, even if the window
							isn't a fullscreen window, but a real window system window.
  
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
	"Get global rect of window or screen. This is the real position / rectangular area occupied "
	"by a screen or onscreen window, with respect to the origin (0,0) of the desktop coordinate "
	"system. A secondary display screen will have its top-left corner not at (0,0), but at a "
	"proper offset, depending if mirror mode is on or off, and how your displays are arranged in "
	"the display settings dialog. An onscreen window which is not displayed in fullscreen mode "
	"will have its top-left and bottom-right corners properly set. ";
static char seeAlsoString[] = "";	

PsychError SCREENGlobalRect(void)  
{	
	int						screenNumber;
	PsychWindowRecordType	*windowRecord;
	PsychRectType			rect; 
    
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(1));			
	PsychErrorExit(PsychRequireNumInputArgs(1));		 
	PsychErrorExit(PsychCapNumOutputArgs(1));			

	if(PsychIsScreenNumberArg(1)) {
		// Real screen id: Get screens global rect and return it:
		PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
		PsychGetGlobalScreenRect(screenNumber, rect);
		PsychCopyOutRectArg(1, FALSE, rect);
	}
	else if(PsychIsWindowIndexArg(1)) {
		// Window:
		PsychAllocInWindowRecordArg(1, TRUE, &windowRecord);

		//  Onscreen?
		if (PsychIsOnscreenWindow(windowRecord)) {
			PsychOSProcessEvents(windowRecord, 0);
			PsychCopyOutRectArg(1, FALSE, windowRecord->globalrect);
		}
		else {
			PsychCopyOutRectArg(1, FALSE, windowRecord->rect);
		}
	}
	else PsychErrorExitMsg(PsychError_user, "Argument was recognized as neither a window index nor a screen pointer");

	return(PsychError_none);
}
