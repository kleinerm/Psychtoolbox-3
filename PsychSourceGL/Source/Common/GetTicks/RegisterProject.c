/*
	PsychToolbox3/Source/Common/GetTicks/RegisterProject.c		

	PROJECTS: GetTicks

	PLATFORMS:  Only OS X so far.  

	AUTHORS:
		Allen.Ingling@nyu.edu		awi 
	  
	HISTORY:
		7/06/03		awi		Created. 

*/

//begin include once 


#include "GetTicks.h"


PsychError PsychModuleInit(void)
{
//        InitializeSynopsis();  //first initialize the list of synopsis strings.
        
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychRegister(NULL,  &GETTICKSGetTicks);

	//register the module name
	PsychErrorExit(PsychRegister("GetTicks", NULL));
        
	//report the version when called with "Version"
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module exit function
	//PsychRegisterExit(&PsychHIDCleanup);
	PsychSetModuleAuthorByInitials("awi");

	return(PsychError_none);

}




