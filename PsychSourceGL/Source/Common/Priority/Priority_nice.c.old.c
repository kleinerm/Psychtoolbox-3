/*
	PsychToolbox3/Source/Common/Priority/Priority.c
	
	PLATFORMS:	All
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	3/31/03			awi		wrote it.  
	

*/


#include "Priority.h"


PsychError PRIORITYPriority(void) 
{
    double 		*returnValue;
    pid_t		processID;
    int			oldPriority, newPriority;

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    //Allocate a return matrix and load it with the current process priority.  
    PsychAllocOutDoubleArg(1, FALSE, &returnValue);
    processID= getpid();
    oldPriority=getpriority(PRIO_PROCESS, processID);
    *returnValue=(double)oldPriority;
    
    //Fetch the input argument and set the priority
    if(PsychCopyInIntegerArg(1, FALSE, &newPriority))
        setpriority(PRIO_PROCESS, processID, newPriority);
    
    return(PsychError_none);	
}




	
