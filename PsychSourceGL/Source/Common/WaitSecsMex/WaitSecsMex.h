/*
	PsychToolbox3/Source/Common/WaitSecsMex/WaitSecsMex.h

	PLATFORMS:	Matlab only
		

	AUTHORS:

		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:

		8/06/02		awi		wrote it.
		7/07/04		awi		Cosmetic
		 
	DESCRIPTION 

		WaitSecsMex is a variant of WaitSecs which strips out the Psychtoolbox mex/mx abstraction layer.
		The purposes are to provide a simplified example to Mathworks and to verify that the psychtoolbox mex/mx abstraction layer
		is not the source of trouble.  

	TARGET LOCATION:

		WaitSecsMex.mexmac resides in:
			PsychToolbox/PsychObsolete/PsychBasicOSX/WaitSecs.mexmac
			
		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/WaitSecsMex_CopyOut.sh
	
*/


#include "mex.h"
#include <CoreAudio/HostTime.h>


	
