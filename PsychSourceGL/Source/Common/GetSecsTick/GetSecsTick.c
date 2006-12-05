/*
	PsychToolbox3/Source/Common/GetSecsTick/GetSecsTick.c
	
	PLATFORMS:	All
			
	PROJECTS: GetSecsTick only.  
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	1/20/02			awi		wrote it.  
	

*/


#include "GetSecsTick.h"


PsychError GETSECSTICKGetSecsTick(void) 
{
    double 	*returnValue, clockFrequency;  

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
    //Allocate a return matrix and load it with the depth values.  
    PsychGetPrecisionTimerTicksPerSecond(&clockFrequency);
    PsychAllocOutDoubleArg(1, FALSE, &returnValue);
    *returnValue=1/clockFrequency; 
    
    return(PsychError_none);	
}




	
