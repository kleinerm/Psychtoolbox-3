/*
	PsychSourceGL/Source/Common/CocoaEventBridge/COCOAEVENTBRIDGECharAvail.c		
  
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

static char useString[] = "avail=CocoaEventBridge('CharAvail');";
static char synopsisString[] = 
  "Return 1 if a character is available in the event queue, 0 if not. Note "
  "that this routine leaves the character in the queue.  Call GetChar to "
  "remove the character from the event queue."
  "\n\n"
  "Do not call CocoaEventBridge('CharAvail') directly.  Instead, use the Psychtoolbox function \"CharAvail\".";

static char seeAlsoString[] = "";	


PsychError COCOAEVENTBRIDGECharAvail(void) 
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
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
	//Open the window.  OpenGetCharWindow() and MakeGetCharWindowInvisible() only act once if called repeatedly.
	OpenGetCharWindow();
	#ifndef DEBUG_USE_VISIBLE_WINDOW
	MakeGetCharWindowInvisible();
	#endif

	numKeypresses=GetNumKeypresses();
	charAvail= numKeypresses > 0;
	PsychCopyOutFlagArg(1, kPsychArgOptional, charAvail);

    return(PsychError_none);	
}






	
