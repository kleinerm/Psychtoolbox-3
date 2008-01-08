/*
	PsychToolbox3/Source/Common/PsychCV/RegisterProject.c		
  
	PROJECTS: PsychCV only.
  
	AUTHORS:
	
	Mario Kleiner	mk		mario.kleiner at tuebingen.mpg.de
	
	PLATFORMS:
	
	This file should compile on all platforms.  
    
	HISTORY:

	5.1.2008		mk		Created.
	
	TARGET LOCATION:

	To change the target location modify the script:
	Psychtoolbox/Tools/Scripts/CopyOutMexFiles/DoNothing_CopyOut.sh

*/

#include "Psych.h"
#include "PsychCV.h"

PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(&PsychCVExit)); 
	
	// Register the project function which is called when the module
	// is invoked with no arguments:
	PsychErrorExit(PsychRegister(NULL,  &PSYCHCVDisplaySynopsis));
        
	// Report the version
	#if PSYCH_SYSTEM == PSYCH_OSX
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));
	#endif

	// Register the module name
	PsychErrorExit(PsychRegister("PsychCV", NULL));
	
	// Register synopsis and named subfunctions.
	PsychErrorExit(PsychRegister("Verbosity", &PSYCHCVVerbosity));
	PsychErrorExit(PsychRegister("OpenEyesInitialize", &PSYCHCVOpenEyesInitialize));
	PsychErrorExit(PsychRegister("OpenEyesShutdown", &PSYCHCVOpenEyesShutdown));
	PsychErrorExit(PsychRegister("OpenEyesParameters", &PSYCHCVOpenEyesParameters));
	PsychErrorExit(PsychRegister("OpenEyesTrackEyePosition", &PSYCHCVOpenEyesTrackEyePosition));

//	PsychErrorExit(PsychRegister("Stop", &PSYCHPORTAUDIOStopAudioDevice));
//	PsychErrorExit(PsychRegister("FillBuffer", &PSYCHPORTAUDIOFillAudioBuffer));
//	PsychErrorExit(PsychRegister("GetDevices", &PSYCHPORTAUDIOGetDevices));
//	PsychErrorExit(PsychRegister("GetStatus", &PSYCHPORTAUDIOGetStatus));
//	PsychErrorExit(PsychRegister("LatencyBias", &PSYCHPORTAUDIOLatencyBias));
//	PsychErrorExit(PsychRegister("GetAudioData", &PSYCHPORTAUDIOGetAudioData));
	
	// Setup synopsis help strings:
	InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.

	// Setup module author:
	PsychSetModuleAuthorByInitials("mk");

	// Call wait-routine for 0.1 secs: This to initialize the time glue on MS-Windows,
	// so the first call to a timing function won't delay:
	PsychWaitIntervalSeconds(0.1);

	// Perform all remaining initialization:
	PsychCVInitialize();

	// Startup finished.
	return(PsychError_none);
}
