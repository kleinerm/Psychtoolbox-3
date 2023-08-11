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
#include "PsychCVARToolkit.h"

PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(&PsychCVExit)); 
	
	// Register the project function which is called when the module
	// is invoked with no arguments:
	PsychErrorExit(PsychRegister(NULL,  &PSYCHCVDisplaySynopsis));
        
	// Report the version
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	// Register the module name
	PsychErrorExit(PsychRegister("PsychCV", NULL));
	
	// Register synopsis and named subfunctions.
	PsychErrorExit(PsychRegister("Verbosity", &PSYCHCVVerbosity));
	#ifdef PSYCHCV_USE_OPENCV
	PsychErrorExit(PsychRegister("OpenEyesInitialize", &PSYCHCVOpenEyesInitialize));
	PsychErrorExit(PsychRegister("OpenEyesShutdown", &PSYCHCVOpenEyesShutdown));
	PsychErrorExit(PsychRegister("OpenEyesParameters", &PSYCHCVOpenEyesParameters));
	PsychErrorExit(PsychRegister("OpenEyesTrackEyePosition", &PSYCHCVOpenEyesTrackEyePosition));
	#endif
	
	PsychErrorExit(PsychRegister("CopyMatrixToMemBuffer", &PSYCHCVCopyMatrixToMemBuffer));

	PsychErrorExit(PsychRegister("ARInitialize", &PSYCHCVARInitialize));
	PsychErrorExit(PsychRegister("ARShutdown", &PSYCHCVARShutdown));
	PsychErrorExit(PsychRegister("ARLoadMarker", &PSYCHCVARLoadMarker));
	PsychErrorExit(PsychRegister("ARDetectMarkers", &PSYCHCVARDetectMarkers));
	PsychErrorExit(PsychRegister("ARRenderImage", &PSYCHCVARRenderImage));
	PsychErrorExit(PsychRegister("ARTrackerSettings", &PSYCHCVARTrackerSettings));
	PsychErrorExit(PsychRegister("ARRenderSettings", &PSYCHCVARRenderSettings));

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
