/*
	PsychToolbox3/Source/Common/MachPriorityMex/MachPriorityMex.h
	
	PLATFORMS:	OS X only
			
	AUTHORS:

		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:

		8/08/02		awi		wrote it.
		7/07/04		awi		Cosmetic

        
	DESCRIPTION: 
	
		MachSetPriorityMex and MachGetPriorityMex are variants of MachPriority which strip out the Psychtoolbox mex/mx abstraction layer.
		The purpose of doing that was to provide a simplified example to Mathworks and to verify that the psychtoolbox mex/mx abstraction layer
		was not the source of the timing bug.  
		
	TARGET LOCATIONS:

		MachSetPriorityMex.mexmac resides in:
			PsychToolbox/PsychBasic/MachSetPriorityMex.mexmac

		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/MachSetPriorityMex_CopyOut.sh


		MachGetPriorityMex.mexmac resides in:
			PsychToolbox/PsychBasic/MachGetPriorityMex.mexmac
			
		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/MachGetPriorityMex_CopyOut.sh
	

*/


#include "mex.h"
#include <CoreAudio/HostTime.h>


//special includes for Priority mach threads version
#include <mach/mach_init.h>
#include <mach/task_policy.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
#include <sys/sysctl.h>
	
   
#define COMMAND_STRING_LENGTH		128	
        
//MachPrioirityCommonMex.c
char *GetFlavorStringFromFlavorConstant(thread_policy_flavor_t flavor);				
bool GetFlavorConstantFromFlavorString(char *flavorString, int flavorStringLength, thread_policy_flavor_t *flavorConstant);  //case sensitive.  

