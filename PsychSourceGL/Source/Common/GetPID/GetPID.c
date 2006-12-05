/*
    PsychToolbox3/Source/Common/GetPID/GetPID.c		
  
    PROJECTS: GetPID only. 
  
    PLATFORMS:	Currently OS X only
 
    AUTHORS:
    Allen.Ingling@nyu.edu		awi 
  
    PLATFORMS:	Currently OS X only
    
    HISTORY:
    2/28/04  awi		Created. 
 
  
    DESCRIPTION: 
  

*/


#include "GetBusTicks.h"
#include <sys/types.h>
#include <unistd.h>



PsychError GETPIDGetPID(void) 
{
    pid_t   pid;
 
    	 
    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0)); //actually we permit only zero or three arguments.
    
    pid=getpid();
    PsychCopyOutDoubleArg(1, FALSE, (double)pid);
    
    return(PsychError_none);	
}




	
