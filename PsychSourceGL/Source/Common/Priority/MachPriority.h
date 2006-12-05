/*
	PsychToolbox3/Source/Common/MachPriority/MachPriority.h
	
	PLATFORMS:	All
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
		3/31/03		awi		wrote it.
	
	DESCRIPTION:
		Read and set  priority.   This function has been superceded by MachGetPriorityMex and MachSetPriorityMex functions which do better jobs of reading
		and setting priority but do not use the PsychtScriptingGlue layer.  The mex/mx API was used instead to make these functions more transparent 
		to Mathworks programmers doing work on the timing bug.  Because we understand that now, MachGetPriority and MachSetPriority could be re-written
		to use PsychScriptingGlue.

	TO DO:
		Because we don't yet have abstracted functions for reading in struct arrays we accept 
		the inputs as seperate arguments.  This should be changed to also permit a struct array.
		 
		Cache the original priority and restore it when this module is purged.
		
	NOTES:
	    
	

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_Priority
#define PSYCH_IS_INCLUDED_Priority

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"

//special includes for old nice version of Priority (see priority.c.old)
/*
#include <sys/types.h>
#include <unistd.h>
#include <sys/resource.h>
*/

//special includes for Priority pthreads version
/*
#include <pthread.h>
*/

//special includes for Priority mach threads version
#include <mach/mach_init.h>
#include <mach/task_policy.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#include <sys/sysctl.h>



/*  from Apple Example
#include <mach/mach_init.h>
#include <mach/thread_policy.h>
#include <mach/sched.h>
*/




//function prototypes
PsychError MODULEVersion(void); 
PsychError MACHPRIORITYMachPriority(void);

//end include once
#endif

	
