/*
	PsychSourceGL/Source/Common/CocoaEventBridge/COCOAEVENTBRIDGEListenForChars.c
  
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

static char useString[] = "CocoaEventBridge('ListenChars');";
static char synopsisString[] = 
        "Begin storing keystrokes into the CocoaEventBridge event queue."
		"\n\n"
		"Ordinarily, keystroke storage begins when CocoaEventBridge('GetChar') is called and ends when the same "
		"function returns after detecting a keypress.  If you want CocoaEventBridge('GetChar') to detect keystrokes "
		"made before it was called, then call CocoaEventBridge('ListenChars') at the time when you want to begin "
		"storing keystrokes into the queue." 
		"\n\n"
		"In OS 9,  keystrokes are directed into the GetChar queue while your script or functions executes in the MATLAB environment. "
		"In OS X, keystrokes are dircted to the  CocoaEventBridge('GetChar') queue while your script or function executes beginning from the "
		"time when that script of function calls CocoaEventBridge('ListenChars').  Note that CocoaEventBridge('GetChar') automatically begins "
		"listening for characters when called; calling CocoaEventBridge('ListenChars') immediatly before CocoaEventBridge('GetChar') is unnecessary."
		"\n\n"
		"Do not call CocoaEventBridge('ListenChars') directly.  Instead, call \"ListenChars\".";


static char seeAlsoString[] = "";	


PsychError COCOAEVENTBRIDGEListenForChars(void) 
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

	StartKeyGathering();
	MakeKeyWindow();

    return(PsychError_none);	
}






	
