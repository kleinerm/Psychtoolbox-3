/*
    PsychToolbox3/Source/Common/GetBusTicks/RegisterProject.c		
  
    PROJECTS: GetBusTicks only. 
  
    PLATFORMS:	Currently OS X only
 
    AUTHORS:
    Allen.Ingling@nyu.edu		awi 
  
    PLATFORMS:	Currently OS X only
    
    HISTORY:
    4/14/03  awi		Created. 
 
  
    DESCRIPTION: 
  
  
  

*/

//begin include once 


#include "Psych.h"
#include "GetBusTicks.h"



PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no named subfunction:
	PsychErrorExit(PsychRegister(NULL,  &GETBUSTICKSGetBusTicks));
        
        //report the version when called with "Version"
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("GetBusTicks", NULL));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments.  
	PsychSetModuleAuthorByInitials("awi");

	return(PsychError_none);

}




