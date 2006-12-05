/*
	PsychToolbox3/Source/Common/TestProirityMex/TestPriorityMex.h		

	PROJECTS: TestPriority only.

	PLATFORMS:  Only OS X so far.  

	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
	  
	HISTORY:

		4/21/03		awi		Created.
		9/22/03		awi		Rewrote to use same arguments as TimeTest standalone application and replaced ScriptingGlue calls with mx/mex API calls.
		7/07/04		awi		Cosmetic

	DESCRIPTION:

		The function accepts the following arguments:

			position	description					size
				1	policy period					1x1
				2	policy computation				1x1
				3	policy constraint				1x1
				4	policy preemptible				1x1
				5	blocking period in seconds		1x1
				6	test  duration in seconds		1x1	

	TARGET LOCATION:

		TestPriorityMex.mexmac resides in:
			PsychToolbox/PsychObsolete/PsychBasicOSX/TestPriorityMex.mexmac
			
		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/TestPriorityMex_CopyOut.sh


*/



#include "mex.h"
#include <CoreAudio/HostTime.h>
#include <unistd.h>


	
#include <mach/mach_types.h>
#include <math.h>
#include <CoreServices/CoreServices.h>
#include <mach/mach_init.h>
#include <mach/task_policy.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#include <sys/sysctl.h>

