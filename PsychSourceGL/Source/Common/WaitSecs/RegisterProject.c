/*
  PsychToolbox3/Source/Common/WaitSecs/RegisterProject.c		
  
  PROJECTS: GetSecs only.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	This file should compile on all platforms.  
    
  HISTORY:
  8/20/02  awi		Created. 
  1/20/02  awi		Created derived the GetSecs version from the Screen version.  
 
  

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
        
	//report the version
        #if PSYCH_SYSTEM == PSYCH_OSX
	PsychErrorExit(PsychRegister("Version", &MODULEVersion));
	#endif

	//register the module name
	PsychErrorExit(PsychRegister("WaitSecs", NULL));
	
	PsychSetModuleAuthorByInitials("awi");
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.  

	return(PsychError_none);

}




