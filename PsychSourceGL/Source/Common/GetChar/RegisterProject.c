/*
PsychToolbox3/Source/Common/CocoaEventBridge/RegisterProject.c		

	PROJECTS: 

		CocoaEventBridge only.

	PLATFORMS:  
	
		Only OS X.  

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
	  
	HISTORY:
	
		9/14/05  awi		Created. 

*/

//begin include once 


#include "CocoaEventBridge.h"



PsychError PsychModuleInit(void)
{
//        InitializeSynopsis();  //first initialize the list of synopsis strings.
        
	//register the project function which is called when the module
	PsychRegister("GetChar",  &COCOAEVENTBRIDGEGetChar);
	PsychRegister("CharAvail",  &COCOAEVENTBRIDGECharAvail);
	PsychRegister("FlushChars", &COCOAEVENTBRIDGEFlushChars);
	PsychRegister("ListenChars", &COCOAEVENTBRIDGEListenForChars);
	PsychRegister("StopListenChars", &COCOAEVENTBRIDGEStopListenForChars);
	PsychRegister("RevertKeyWindow", &COCOAEVENTBRIDGERevertKeyWindow);
	PsychRegister("PathToBundle", &COCOAEVENTBRIDGEPathToBundle);

	
	//register the module name
	PsychErrorExit(PsychRegister("CocoaEventBridge", NULL));
        
	//report the version when called with "Version"
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));
	
	//register the project function which is called when the module
	//is invoked with no arguments:
	//PsychRegister(NULL,  &DisplayScreenSynopsis)
	PsychRegister(NULL, &DisplayEventBridgeSynopsis);

	
	//register the exit function.
	PsychRegisterExit(&CocoaEventBridgeExitFunction);

	PsychSetModuleAuthorByInitials("awi");

	//register the module exit function
	//PsychRegisterExit(&PsychHIDCleanup);

	return(PsychError_none);

}




