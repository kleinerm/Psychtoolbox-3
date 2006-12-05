/*
	RegisterProject.c		

	PROJECTS:
	
		Gestalt only

	PLATFORMS:  
	
		Only OS X

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
	  
	HISTORY:
		
		12/6/04		awi		Created.
		
  
  
*/

//begin include once 


#include "Gestalt.h"



PsychError PsychModuleInit(void)
{
//        InitializeSynopsis();  //first initialize the list of synopsis strings.
        
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychRegister(NULL,  &GESTALTGestalt);

	//register the module name
	PsychErrorExit(PsychRegister("Gestalt", NULL));
        
	//report the version when called with "Version"
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	PsychSetModuleAuthorByInitials("awi");

	//register the module exit function
	//PsychRegisterExit(&PsychHIDCleanup);

	return(PsychError_none);

}




