/*
    PsychToolbox3/Source/Common/GetBusTicks/GetBusTicks.c
	
    PROJECTS: GetBusTicks only. 
  
    PLATFORMS:	Currently OS X only
	
    AUTHORS:
    Allen Ingling	awi		Allen.Ingling@nyu.edu

    HISTORY:
    4/16/03		awi		wrote it.  
	

*/


#include "GetBusTicks.h"


PsychError GETBUSTICKSGetBusTicks(void) 
{
    uint64_t	timeValue; 
    	 
    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0)); //actually we permit only zero or three arguments.
    
    timeValue=mach_absolute_time();
    //clock_get_uptime(&timeValue);

    
    PsychCopyOutDoubleArg(1, FALSE, (double)timeValue);
    
    return(PsychError_none);	
}




	
