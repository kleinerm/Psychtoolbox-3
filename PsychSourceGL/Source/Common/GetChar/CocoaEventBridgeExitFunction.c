/*
	PsychSourceGL/Source/Common/CocoaEventBridge/CocoaEventBridgeExitFunction.c		
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/14/05		awi		Wrote it .


*/


#include "EventBridgeBundleHeader.h"
#include "CocoaEventBridge.h"


void FreePathToBundleString(void);


PsychError CocoaEventBridgeExitFunction(void)
{

	if(CloseGetCharWindow != NULL)
		CloseGetCharWindow();
		
	FreePathToBundleString();
		
}

	
