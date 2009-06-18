/*
	PsychSourceGL/Source/Common/CocoaEventBridge/COCOAEVENTBRIDGEFlushChars.c	
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/15/05		awi		Wrote it .


*/

#include "EventBridgeBundleHeader.h"
//#include "CocoaEventBridgeHelpers.h"
#include "CocoaEventBridge.h"

static char useString[] = "CocoaEventBridge('FlushChars');";
static char synopsisString[] = 
	"Removes all characters from the event queue."
	"\n\n"
	"Do not call CocoaEventBridge('FlushChars') directly.  Instead, call \"FlushEvents('keyDown');\".";

static char seeAlsoString[] = "";	


PsychError COCOAEVENTBRIDGEFlushChars(void) 
{

	psych_bool						loadBundleError, charAvail;
	int							numKeypresses;
	

	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	loadBundleError=LoadCocoaBundle();
	if(loadBundleError)
		PsychErrorExitMsg(PsychError_internal, "Failed to load the cocoa bundle.");
		
    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
	//Open the window.  OpenGetCharWindow() and MakeGetCharWindowInvisible() only act once if called repeatedly.
	OpenGetCharWindow();
	#ifndef DEBUG_USE_VISIBLE_WINDOW
	MakeGetCharWindowInvisible();
	#endif

	ClearKeypressList();
	
    return(PsychError_none);	
}






	
