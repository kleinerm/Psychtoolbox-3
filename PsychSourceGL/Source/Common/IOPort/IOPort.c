/*
	IOPort.c		
  
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


PsychError IOPORTIOPort(void)
{

    //check to see if the user supplied superfluous arguments
    //PsychErrorExit(PsychCapNumOutputArgs(1));
    //PsychErrorExit(PsychCapNumInputArgs(0));
    
    int 	result;
    UInt32 	buffer;
	
	buffer=0;

    // Initializing IOWarrior Library
    printf ("Calling IOWarriorInit\n");
    result = IOWarriorInit ();
    if (result)
    {
        printf ("IOWarriorInit returned %d\n", result);
        return -1;
    };

    // checking for presence
    if (IOWarriorIsPresent ())
        printf ("IOWarrior is present\n");
    else
        printf ("IOWarrior is not present\n");

    // writing 32 bits
    buffer = (1) << 3;
    result = IOWarriorWriteInterface0 (&buffer);
    if (result)
        printf ("IOWarriorWriteInterface0 returned %d\n", result);
	
    return(PsychError_none);	
}




	
