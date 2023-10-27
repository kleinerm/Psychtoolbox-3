/*
    PsychToolbox3/Source/Common/GetPID/GetPID.h		
  
    PROJECTS: GetPID only. 
  
    PLATFORMS:	Currently OS X only
 
    AUTHORS:
    Allen.Ingling@nyu.edu		awi 
  
    PLATFORMS:	Currently OS X only
    
    HISTORY:
    2/28/04  awi		Created. 
 
  
    DESCRIPTION: 
  

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_GetPID
#define PSYCH_IS_INCLUDED_GetPID

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"
#include <sys/types.h>
#include <unistd.h>


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
PsychError GETPIDGetPID(void) ;

//end include once
#endif

	
