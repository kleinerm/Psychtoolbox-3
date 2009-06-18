/*
	PsychSourceGL/Source/Common/CocoaEventBridge/COCOAEVENTBRIDGEStopListenForChars.c
  
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

static char useString[] = "CocoaEventBridge('StopListenChars');";
static char synopsisString[] = 
        "Stop storing keystrokes into the CocoaEventBridge event queue."
		"\n\n"
		"Ordinarily, keystroke storage begins when CocoaEventBridge('GetChar') is called and ends when the same "
		"function returns after detecting a keypress.  If you want CocoaEventBridge('GetChar') to detect keystrokes "
		"made before it was called, then call CocoaEventBridge('ListenChars') at the time when you want to begin "
		"storing keystrokes into the queue. To stop storing keystrokes into the queue, call CocoaEventBridge('StopListenChars')." 
		"\n\n"
		"In OS 9,  keystrokes are directed into the GetChar queue while your script or functions executes in the MATLAB environment. "
		"In OS X, keystrokes are directed to the  CocoaEventBridge('GetChar') queue while your script or function executes beginning from the "
		"time when that script of function calls CocoaEventBridge('ListenChars').  Note that CocoaEventBridge('GetChar') automatically begins "
		"listening for characters when called; calling CocoaEventBridge('ListenChars') immediatly before CocoaEventBridge('GetChar') is unnecessary."
		"\n\n"
		"Do not call CocoaEventBridge('StopListenChars') directly.  Instead, call \"StopListenChars\".";
static char seeAlsoString[] = "";	


PsychError COCOAEVENTBRIDGEStopListenForChars(void) 
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
	
	StopKeyGathering();
	if(IsKeyWindow())
		RevertKeyWindow();


    return(PsychError_none);	
}






	
