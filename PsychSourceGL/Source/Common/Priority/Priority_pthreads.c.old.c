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
    //pid_t		processID;
    static int		oldPriority;
    int			newPriority;
    boolean		realtimePosix=FALSE;
    pthread_t		threadID;
    int			maxPriority, minPriority, currentPolicy, pError, bigNumberPriority, littleNumberPriority;
    int			policies[]={SCHED_FIFO, SCHED_RR, SCHED_OTHER};
    struct sched_param	scheduleParam;
    
    			

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    #ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
        realtimePosix=TRUE;
    #endif
    
    
    //Allocate a return matrix and load it with the current process priority.  
    PsychAllocOutDoubleArg(1, FALSE, &returnValue);
    
    //processID= getpid();
    //oldPriority=getpriority(PRIO_PROCESS, processID);
    //*returnValue=(double)oldPriority;
    
    //Fetch the input argument and set the priority
    //if(PsychCopyInIntegerArg(1, FALSE, &newPriority))
        //setpriority(PRIO_PROCESS, processID, newPriority);
        
    //gather environment info and return the schedule priority
    threadID=pthread_self();
    pthread_getschedparam(threadID, &currentPolicy, &scheduleParam);
    *returnValue=(double)scheduleParam.sched_priority; 
    
    //get the input argument sanity check it
    maxPriority=sched_get_priority_max(currentPolicy);
    minPriority=sched_get_priority_min(currentPolicy);
    bigNumberPriority= maxPriority > minPriority ? maxPriority : minPriority;
    littleNumberPriority= minPriority < maxPriority ? minPriority : maxPriority; 
    if(PsychCopyInIntegerArg(1, FALSE, &newPriority)){
        if(newPriority > bigNumberPriority)
            PsychErrorExitMsg(PsychError_generalUsage, "Specified priority value is too high.  MostPriority and LeastPriority return bounds.");
        if(newPriority < littleNumberPriority)
            PsychErrorExitMsg(PsychError_generalUsage, "Specified priority value is too low.  MostPriority and LeastPriority return bounds.");
        scheduleParam.sched_priority=newPriority;
        pError=pthread_setschedparam(threadID, SCHED_FIFO, &scheduleParam);    
     }       
    
    return(PsychError_none);	
}




	
