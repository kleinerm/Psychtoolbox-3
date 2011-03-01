/*
  PsychToolbox3/Source/Common/WaitSecs/RegisterProject.c		
  
  PROJECTS: WaitSecs only.  
  
  AUTHORS:
  		Allen.Ingling@nyu.edu		awi 
  		Mario Kleiner			mk		mario.kleiner@tuebingen.mpg.de

  PLATFORMS:	This file should compile on all platforms.  

  HISTORY:
  8/20/02  awi		Created. 
  1/20/02  awi		Created derived the GetSecs version from the Screen version.  
  1/2/08   mk		Add subfunction for waiting until absolute time, and return of wakeup time. 

  DESCRIPTION: 

*/

//begin include once 
#include "Psych.h"
#include "WaitSecs.h"

PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no named subfunction:
	PsychErrorExit(PsychRegister(NULL,  &WAITSECSWaitSecs));
        
	// Wait until specific deadline:
	PsychErrorExit(PsychRegister("UntilTime", &WAITSECSWaitUntilSecs));
	PsychErrorExit(PsychRegister("YieldSecs", &WAITSECSYieldSecs));
	
	//report the version
	PsychErrorExit(PsychRegister("Version", &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("WaitSecs", NULL));
	
	PsychSetModuleAuthorByInitials("awi");
	PsychSetModuleAuthorByInitials("mk");

	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.  

	return(PsychError_none);

}
