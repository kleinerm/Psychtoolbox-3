/*
	PsychSourceGL/Source/Experiments/StoreBit/RegisterProject.c		
  
	PROJECTS: 
  
		StoreBit only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/7/05		awi		Wrote it .


*/


#include "Psych.h"
#include "StoreBit.h"




PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychRegisterExit(&StoreBitExitFunction);
//	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychErrorExit(PsychRegister(NULL,  &STOREBITStoreBit));
        
        //report the version
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("StoreBit", NULL));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments. 
	PsychSetModuleAuthorByInitials("awi");
 

	return(PsychError_none);

}




