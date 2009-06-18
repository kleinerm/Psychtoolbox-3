/*
	PsychSourceGL/Source/Common/CocoaEventBridge/CocoaEventBridgeHelpers.h		
  
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


//function prototypes
psych_bool LoadCocoaBundle(void);

//functions loaded from the Cocoa bundle.  Helpful to call LoadCocoaBundle() before using these.  
typedef	void (*InitializeCocoaProc)(void);
typedef	void (*OpenGetCharWindowProc)(void);
typedef	void (*CloseGetCharWindowProc)(void);
typedef	void (*MakeGetCharWindowVisibleProc)(void);
typedef	void (*MakeGetCharWindowInvisibleProc)(void);
typedef	void (*StartKeyGatheringProc)(void);
typedef	void (*StopKeyGatheringProc)(void);
typedef	void (*MakeKeyWindowProc)(void);
typedef	void (*RevertKeyWindowProc)(void);
typedef	CFArrayRef (*CopyReadKeypressListProc)(void);
typedef	CFArrayRef (*CopyPeekKeypressListProc)(void);
typedef	CFDictionaryRef (*CopyReadNextKeypressProc)(void);
typedef	CFDictionaryRef (*CopyPeekNextKeypressProc)(void);
typedef	void (*ClearKeypressListProc)(void);
typedef	int (*GetNumKeypressesProc)(void);
typedef	psych_bool	(*IsKeyWindowProc)(void);





	
