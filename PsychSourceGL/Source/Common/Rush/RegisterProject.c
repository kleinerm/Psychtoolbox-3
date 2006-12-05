/*
  PsychToolbox3/Source/Common/Rush/RegisterProject.c		
  
  PROJECTS: Rush only  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	This file should compile on all platforms.  
    
  HISTORY:
  12/11/03  awi		Derived from Denis Pelli's Rush for the OS 9.  
 
  
  DESCRIPTION: 
  

*/

#include "Psych.h"
#include "Rush.h"




PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked without a sufunction name. 
	PsychErrorExit(PsychRegister(NULL,  &RUSHRush));
        
    //report the version
	//PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("Rush", NULL));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.  

	return(PsychError_none);

}




