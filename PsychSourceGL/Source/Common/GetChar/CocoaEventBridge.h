/*
  PsychSourceGL/Source/Common/CocoaEventBridge/GetChar.h		
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/14/05		awi		Wrote it .


*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_CocoaEventBridge
#define PSYCH_IS_INCLUDED_CocoaEventBridge

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"

//external function prototypes
PsychError MODULEVersion(void); 
PsychError COCOAEVENTBRIDGEGetChar(void); 
PsychError COCOAEVENTBRIDGECharAvail(void);
PsychError COCOAEVENTBRIDGEFlushChars(void);
PsychError COCOAEVENTBRIDGEListenForChars(void);
PsychError COCOAEVENTBRIDGEStopListenForChars(void);
PsychError COCOAEVENTBRIDGERevertKeyWindow(void);
PsychError COCOAEVENTBRIDGEPathToBundle(void);
PsychError CocoaEventBridgeExitFunction(void);
PsychError DisplayEventBridgeSynopsis(void);

void GetPathToBundleString(char **fPath);

//#define DEBUG_USE_VISIBLE_WINDOW

//end include once
#endif

	
