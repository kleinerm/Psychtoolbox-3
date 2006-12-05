/*
	PsychToolbox3/Source/Common/GetBusFrequencyMex/GetBusFrequencyMex.h
	
	PLATFORMS:	Matlab on OSX only
			
	
	AUTHORS:

		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:

		8/12/03		awi		wrote it.
		7/07/04		awi		Cosmetic
        
	DESCRIPTION 
        
		GetBusFrequencyMex returns the system bus frequency. Values read and written by MachGetPriorityMex and
		MachSetPriorityMex when specifying THREAD_TIME_CONSTRAINT_POLICY (realtime) paramters are in units of bus
		ticks.  Use the tick rate as returned by GetBusFrequencyMex to convert units into seconds.    

	TARGET LOCATION:

		GetBusFrequency.mexmac resides in:
			PsychToolbox/PsychObsolete/PsychBasic/
			
		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/GetBusFrequency_CopyOut.sh
  


*/


#include "mex.h"
#include <sys/sysctl.h>

	
