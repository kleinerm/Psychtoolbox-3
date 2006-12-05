/*
	RegisterProject.c		
  
	PROJECTS: 
	
		IOPort only.  
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
	
		Only OS X for now
    
	HISTORY:

		12/20/2004	awi		Wrote it.
 
	DESCRIPTION:
	
		Controls the Code Mercenaries IO Warrior 40 device.  
	
  
  

*/

//begin include once 


#include "Psych.h"
#include "IOPort.h"


// PsychModuleInit is in Screen.cpp
// PsychProjectExit is in Screen.cpp
// PsychProjectSelectFunction is in Screen.cpp


PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no arguments:
//	PsychErrorExit(PsychRegister(NULL,  &IOPORTIOPort));
        
	//report the version
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("IOPort", NULL));
	
	//register subfunctions
	PsychErrorExit(PsychRegister("Init",  &IOPORTInit));
	PsychErrorExit(PsychRegister("SetPort",  &IOPORTSetPort));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments. 
	PsychSetModuleAuthorByInitials("awi");
 

	return(PsychError_none);

}




