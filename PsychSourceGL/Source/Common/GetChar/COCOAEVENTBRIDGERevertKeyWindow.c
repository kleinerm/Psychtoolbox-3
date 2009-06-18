/*
	PsychSourceGL/Source/Common/CocoaEventBridge/COCOAEVENTBRIDGERevertKeyWindow.c		
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/16/05		awi		Wrote it .


*/

#include "EventBridgeBundleHeader.h"
//#include "CocoaEventBridgeHelpers.h"
#include "CocoaEventBridge.h"

static char useString[] = "[char, when]=CocoaEventBridge('RevertKeyWindow');";
static char synopsisString[] = 
        "CocoaEventBridge('GetChar') directs keyboard input away from MATLAB windows. CocoaEventBridge('RevertKeyWindow') returns focus."
		"\n\n"
		"Do not call CocoaEventBridge('RevertKeyWindow') directly.";
static char seeAlsoString[] = "GetChar";	

PsychError COCOAEVENTBRIDGERevertKeyWindow(void) 
{

	psych_bool						loadBundleError;
	

	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(0));

	loadBundleError=LoadCocoaBundle();
	if(loadBundleError)
		PsychErrorExitMsg(PsychError_internal, "Failed to load the cocoa bundle.");
		
	RevertKeyWindow();	
	
    return(PsychError_none);	
}






	
