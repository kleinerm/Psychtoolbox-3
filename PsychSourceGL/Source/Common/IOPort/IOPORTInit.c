/*
	IOPORTInit.c		
  
	PROJECTS: 
	
		IOPort only.
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu				awi
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All.
    
	HISTORY:

		12/20/2004	awi		Wrote it.
		04/10/2008	mk		Started to extend/rewrite it to become a full-fledged generic I/O driver (serial port, parallel port, etc...).
 
	DESCRIPTION:
	
		Originally controlled the Code Mercenaries IO Warrior 40 device.
	
		It will hopefully become a generic I/O driver...

*/

#include "IOPort.h"

#if PSYCH_SYSTEM == PSYCH_OSX
#include <CoreFoundation/CoreFoundation.h>
//#include "IOWarriorLib.h"


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
//    error = IOWarriorInit ();
    if(error)
    {
        sprintf(errorMessage, "IOWarriorInit returned %d\n", error);
		PsychErrorExitMsg(PsychError_internal , errorMessage);
    };

    // checking for presence
//    if (!IOWarriorIsPresent ())
//		PsychErrorExitMsg(PsychError_internal , "Can not find any I/O Warrior device.");
	
    return(PsychError_none);	
}
#endif
