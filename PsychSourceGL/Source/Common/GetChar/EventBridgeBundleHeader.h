/*
	PsychSourceGL/Source/Common/CocoaEventBridge/EventBridgeBundleHeader.h		
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/14/05		awi		Wrote it .


*/
#include <Carbon/Carbon.h>
#include "CocoaEventBridgeHelpers.h"


extern	InitializeCocoaProc				InitializeCocoa;
extern	OpenGetCharWindowProc			OpenGetCharWindow;
extern	CloseGetCharWindowProc			CloseGetCharWindow;
extern	MakeGetCharWindowVisibleProc	MakeGetCharWindowVisible;
extern	MakeGetCharWindowInvisibleProc	MakeGetCharWindowInvisible;
extern	StartKeyGatheringProc			StartKeyGathering;
extern	StopKeyGatheringProc			StopKeyGathering;
extern	MakeKeyWindowProc				MakeKeyWindow;
extern	RevertKeyWindowProc				RevertKeyWindow;
extern	CopyReadKeypressListProc		CopyReadKeypressList;
extern	CopyPeekKeypressListProc		CopyPeekKeypressList;
extern	CopyReadNextKeypressProc		CopyReadNextKeypress;
extern	CopyPeekNextKeypressProc		CopyPeekNextKeypress;
extern	ClearKeypressListProc			ClearKeypressList;
extern	GetNumKeypressesProc			GetNumKeypresses;
extern	IsKeyWindowProc					IsKeyWindow;





	
