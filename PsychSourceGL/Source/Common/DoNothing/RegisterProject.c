/*
  PsychToolbox3/Source/Common/DoNothing/RegisterProject.c		
  
  PROJECTS: GetSecs only.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	This file should compile on all platforms.  
    
  HISTORY:
  8/20/02  awi		Created. Same behavior as Denis Pelli's DoNothing for OS 9, but calls the PsychScriptingGlue API instead of mex API. 
  7/05/03  awi		DoNothing version added. 

*/


#include "Psych.h"
#include "GetSecs.h"



PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychErrorExit(PsychRegister(NULL,  &DONOTHINGDoNothing));
        
        //report the version
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("DoNothing", NULL));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.
	PsychSetModuleAuthorByInitials("awi");
  

	return(PsychError_none);

}




