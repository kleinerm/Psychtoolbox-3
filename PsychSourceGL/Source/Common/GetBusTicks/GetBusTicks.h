/*
    PsychToolbox3/Source/Common/GetBusTicks/GetBusTicks.h
	
    PROJECTS: GetBusTicks only. 
  
    PLATFORMS:	Currently OS X only
	
    AUTHORS:
    Allen Ingling	awi		Allen.Ingling@nyu.edu

    HISTORY:
    4/16/03		awi		wrote it.  
	

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_GetBusTicks
#define PSYCH_IS_INCLUDED_GetBusTicks

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"


//special includes for GetBusTicks
//#include <mach/clock.h>
#include <mach/mach_time.h>

/*
#include <mach/mach_init.h>
#include <mach/task_policy.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#include <sys/sysctl.h>
*/


//function prototypes
PsychError MODULEVersion(void); 
PsychError GETBUSTICKSGetBusTicks(void);

//end include once
#endif

	
