/*
	PsychToolbox3/Source/Common/SysBeep/RegisterProject.c	

	PROJECTS: SysBeep only.

	PLATFORMS:  Only OS X so far.  

	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
	  
	HISTORY:

		7/06/03		awi		Created. 
		7/07/04		awi		Cosmetic

*/

//begin include once 


#include "SysBeep.h"


PsychError PsychModuleInit(void)
{
//        InitializeSynopsis();  //first initialize the list of synopsis strings.
        
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychRegister(NULL,  &SYSBEEPSysBeep);

	//register the module name
	PsychErrorExit(PsychRegister("SysBeep", NULL));
        
	//report the version when called with "Version"
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	PsychSetModuleAuthorByInitials("awi");
	PsychSetModuleAuthorByInitials("dhb");
	PsychSetModuleAuthorByInitials("dgp");
	//register the module exit function
	//PsychRegisterExit(&PsychHIDCleanup);


	return(PsychError_none);

}




