/*
  PsychToolbox3/Source/Common/GetSecsTick/RegisterProject.c		
  
  PROJECTS: GetSecsTick only.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	This file should compile on all platforms.  
    
  HISTORY:
  8/20/02  awi		Created. 
  1/20/02  awi		Created derived the GetSecs version from the Screen version.  
 
 
*/

//begin include once 


#include "Psych.h"
#include "GetSecsTick.h"


// PsychModuleInit is in Screen.cpp
// PsychProjectExit is in Screen.cpp
// PsychProjectSelectFunction is in Screen.cpp


PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychErrorExit(PsychRegister(NULL,  &GETSECSTICKGetSecsTick));
        
	//report the version
	PsychErrorExit(PsychRegister("Version", &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("GetSecsTick", NULL));
	
	PsychSetModuleAuthorByInitials("awi");

	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.  

	return(PsychError_none);

}




