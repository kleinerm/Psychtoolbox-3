/*
	PsychSourceGL/Source/Common/GetSecs/RegisterProject.c		
  
	PROJECTS: 
  
		GetSecs only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X for now.
    
	HISTORY:

		1/20/02		awi		Derived the GetSecs project from Screen .
		4/6/05		awi		Updated header comments.

*/

//begin include once 


#include "Psych.h"
#include "GetSecs.h"


// PsychModuleInit is in Screen.cpp
// PsychProjectExit is in Screen.cpp
// PsychProjectSelectFunction is in Screen.cpp


PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychErrorExit(PsychRegister(NULL,  &GETSECSGetSecs));
        
	//report the version
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("GetSecs", NULL));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments. 
	PsychSetModuleAuthorByInitials("awi");
 

	return(PsychError_none);

}




