/*
	IOPORTInit.c		
  
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


#include "IOPort.h"
#include <CoreFoundation/CoreFoundation.h>
#include "IOWarriorLib.h"


PsychError IOPORTInit(void)
{

    //check to see if the user supplied superfluous arguments
    //PsychErrorExit(PsychCapNumOutputArgs(1));
    //PsychErrorExit(PsychCapNumInputArgs(0));
    
    int			error;
    UInt32		buffer;
	char		errorMessage[256];
	
	buffer=0;

    // Initializing IOWarrior Library
    //printf ("Calling IOWarriorInit\n");
    error = IOWarriorInit ();
    if(error)
    {
        sprintf(errorMessage, "IOWarriorInit returned %d\n", error);
		PsychErrorExitMsg(PsychError_internal , errorMessage);
    };

    // checking for presence
    if (!IOWarriorIsPresent ())
		PsychErrorExitMsg(PsychError_internal , "Can not find any I/O Warrior device.");
	
    return(PsychError_none);	
}




	
