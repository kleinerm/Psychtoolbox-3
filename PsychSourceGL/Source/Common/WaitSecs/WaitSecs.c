/*
	PsychToolbox3/Source/Common/WaitSecs/WaitSecs.c
	
	PLATFORMS:	
	
		Only OS X for now
			
	
	AUTHORS:

		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:

		1/20/02			awi		wrote it.
		4/6/05			awi		Use mach_wait_until() instead of looping.  Mario's suggestion.  
		4/7/05			awi		Relocate mach_wait_until() call within PsychWaitIntervalSeconds().  
		
	
	NOTES: 
	
		We add a small error here by doing some stuff before calling PsychWaitIntervalSeconds.  The error should be small.  
		If we need to do better, mark time from the entry point into WAITSECSWaitSecs().
		
		---
		
		We add a compile flag to the project MachTimebase settings: "-Wno-long-double" turns off a warning caused by using type long double.  
		The warning is:
			use of `long double' type; its size may change in a future release (Long double usage is reported only once for each file.
			To disable this warning, use -Wno-long-double.)
			
		Turning off the warning is probably ok because we do not depend on long doubles being any particular size or precision; if does change,
		that's not going to break anything.  
			
		The -Wno-long-double flag is appended to the project setting "Other Warning Flags".

		   
  
*/


#include "WaitSecs.h"


PsychError WAITSECSWaitSecs(void) 
{
    double	waitPeriodSecs;

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    PsychCopyInDoubleArg(1,TRUE,&waitPeriodSecs);
	 PsychWaitIntervalSeconds(waitPeriodSecs);

    return(PsychError_none);	
}


// The older version, before we moved the part that does the work into our library of Psycthtoolbox core functions.
// We mark time from the entry point.  

/*
PsychError WAITSECSWaitSecs(void) 
{
    double							*waitPeriodSecs;
    long double						clockPeriodNSecs, clockFrequencyHz, waitPeriodTicks;	
    mach_timebase_info_data_t		tbinfo;
	kern_return_t					waitResult;
	uint64_t						startTime, deadline;
	
	startTime = mach_absolute_time();

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    //Allocate a return matrix and load it with the depth values.
    PsychAllocInDoubleArg(1,TRUE,&waitPeriodSecs);
	
    mach_timebase_info(&tbinfo);
    clockPeriodNSecs = ((long double) tbinfo.numer) / ((long double) tbinfo.denom);
    clockFrequencyHz = 1000000000.0 / clockPeriodNSecs;
	waitPeriodTicks= clockFrequencyHz * *waitPeriodSecs;
	deadline= startTime + (uint64_t)waitPeriodTicks;
    waitResult=mach_wait_until(deadline);   

    return(PsychError_none);	
}
*/




	
