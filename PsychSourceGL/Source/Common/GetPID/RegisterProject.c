/*
    PsychToolbox3/Source/Common/GetPID/RegisterProject.c		
  
    PROJECTS: GetPID only. 
  
    PLATFORMS:	Currently OS X only
 
    AUTHORS:
    Allen.Ingling@nyu.edu		awi 
  
    PLATFORMS:	Currently OS X only
    
    HISTORY:
    2/28/04  awi		Created. 
 
  
    DESCRIPTION: 
  

*/

//begin include once 


#include "GetPID.h"

PsychError PsychModuleInit(void)
{
	//register the module exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the module function which is called when the module
	//is invoked with no named subfunction:
	PsychErrorExit(PsychRegister(NULL,  &GETPIDGetPID));
        
	//report the version when called with "Version"
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("GetPID", NULL));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.  
	PsychSetModuleAuthorByInitials("awi");


	return(PsychError_none);

}




