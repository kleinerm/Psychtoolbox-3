/*
  PsychToolbox3/Source/Common/MachPriority/RegisterProject.c		
  
  PROJECTS: MachPriority only.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	This file should compile on all platforms.  
    
  HISTORY:
  8/20/02  awi		Created. 
  1/20/02  awi		Created derived the GetSecs version from the Screen version.  
 


*/

//begin include once 


#include "Psych.h"
#include "MachPriority.h"


// PsychModuleInit is in Screen.cpp
// PsychProjectExit is in Screen.cpp
// PsychProjectSelectFunction is in Screen.cpp


PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no named subfunction:
	PsychErrorExit(PsychRegister(NULL,  &MACHPRIORITYMachPriority));
        
	//report the version when called with "Version"
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("Priority", NULL));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.  
	PsychSetModuleAuthorByInitials("awi");

	return(PsychError_none);

}




