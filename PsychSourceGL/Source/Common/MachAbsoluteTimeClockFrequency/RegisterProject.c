/*
  PsychSourceGL/Source/Common/MachAbsoluteTimeClockFrequency/RegisterProject.c		
  
  PROJECTS: 
  
	MachAbsoluteTimeClockFrequency only.  
  
  AUTHORS:
  
	Allen.Ingling@nyu.edu		awi 
  
    
  HISTORY:
	
	4/5/05				awi		Wrote it.   
 
	

*/

//begin include once 


#include "Psych.h"
#include "MachAbsoluteTimeClockFrequency.h"

PsychError PsychModuleInit(void)
{
	//register the project exit function
	PsychErrorExit(PsychRegisterExit(NULL)); 
	
	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychErrorExit(PsychRegister(NULL,  &MACHABSOLUTETIMECLOCKFREQUENCYMachAbsoluteTimeClockFrequency));
        
        //report the version
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	//register the module name
	PsychErrorExit(PsychRegister("MachAbsoluteTimeClockFrequency", NULL));
	
	//register synopsis and named subfunctions.
	//InitializeSynopsis();   //Scripting glue won't require this if the function takes no arguments. 
	PsychSetModuleAuthorByInitials("awi");
	PsychSetModuleAuthorByInitials("mk");

	return(PsychError_none);

}




