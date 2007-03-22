/*
	PsychToolbox3/Source/Common/PsychPortAudio/RegisterProject.c		
  
	PROJECTS: PsychPortAudio only.
  
	AUTHORS:
	Mario Kleiner	mk		mario.kleiner at tuebingen.mpg.de
	
	PLATFORMS:	This file should compile on all platforms.  
    
	HISTORY:

	21.3.2007	mk		Created.
	
	TARGET LOCATION:

		DoNothing.mexmac resides in:
			PsychToolbox/PsychBasic/
		
		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/DoNothing_CopyOut.sh

*/

#include "Psych.h"
#include "PsychPortAudio.h"

PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(&PsychPortAudioExit)); 
	
	// Register the project function which is called when the module
	// is invoked with no arguments:
	PsychErrorExit(PsychRegister(NULL,  &PSYCHPORTAUDIODisplaySynopsis));
        
	// Report the version
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	// Register the module name
	PsychErrorExit(PsychRegister("PsychPortAudio", NULL));
	
	// Register synopsis and named subfunctions.
	PsychErrorExit(PsychRegister("OpenAudioDevice",  &PSYCHPORTAUDIOOpen));
	PsychErrorExit(PsychRegister("Close",  &PSYCHPORTAUDIOClose));
	PsychErrorExit(PsychRegister("StartAudioDevice",  &PSYCHPORTAUDIOStartAudioDevice));
	PsychErrorExit(PsychRegister("StopAudioDevice",  &PSYCHPORTAUDIOStopAudioDevice));
	PsychErrorExit(PsychRegister("FillAudioBuffer",  &PSYCHPORTAUDIOFillAudioBuffer));

	// Setup synopsis help strings:
	InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.

	// Setup module author:
	PsychSetModuleAuthorByInitials("mk");

	// Startup finished.
	return(PsychError_none);
}
