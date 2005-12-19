/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/RegisterProject.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/21/05  cdb		Created.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "RegisterProject.h"
#include "Psych.h"


PsychError PsychModuleInit(void)
{
	// Initialize Eyelink project variables
	memset(gstrCommand, 0, sizeof(gstrCommand));
	giSystemInitialized = 0;
	
	//
	// TODO:  REGISTER EYELINK EXIT FUNCTION THAT WOULD SHUTDOWN LINK TO EYELINK
	//
	
	// Register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	// Register the project function which is called when the module
	// is invoked with no arguments:
	PsychErrorExitMsg(PsychRegister(NULL,  &PsychDisplayEyelinkSynopsis), "Failed to register the Eyelink synopsis function.");        
	
	// Register the module name
	PsychErrorExitMsg(PsychRegister("Eyelink", NULL), "Failed to register Eyelink Module name!");

	// Report the version
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	// Register sub-commands
	PsychErrorExit(PsychRegister("ButtonStates",		&EyelinkButtonStates));
	PsychErrorExit(PsychRegister("Command",				&EyelinkCommand));
	PsychErrorExit(PsychRegister("CurrentMode",			&EyelinkCurrentMode));
	PsychErrorExit(PsychRegister("ImageModeDisplay",	&EyelinkImageModeDisplay));
	PsychErrorExit(PsychRegister("Initialize",			&EyelinkInitialize));
	PsychErrorExit(PsychRegister("InitWindow",			&EyelinkInitWindow));
	PsychErrorExit(PsychRegister("IsConnected",			&EyelinkIsConnected));
	PsychErrorExit(PsychRegister("NewestFloatSample",	&EyelinkNewestFloatSample));
	PsychErrorExit(PsychRegister("SendKeyButton",		&EyelinkSendKeyButton));
	PsychErrorExit(PsychRegister("Shutdown",			&EyelinkShutdown));
	PsychErrorExit(PsychRegister("StartRecording",		&EyelinkStartRecording));
	PsychErrorExit(PsychRegister("StartSetup",			&EyelinkStartSetup));
	PsychErrorExit(PsychRegister("StopRecording",		&EyelinkStopRecording));
	
	//register synopsis and named subfunctions.
	InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.
	PsychSetModuleAuthorByInitials("cdb");
	PsychSetModuleAuthorByInitials("emp");
	PsychSetModuleAuthorByInitials("fwc");
	
	return(PsychError_none);
}

