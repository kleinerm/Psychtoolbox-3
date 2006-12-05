/*
	PsychToolbox3/Source/Common/GetSecsMex/GetSecsMex.h

	PLATFORMS:	Matlab only
		
	AUTHORS:

		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:

		8/06/02		awi		wrote it. 
		7/07/04		awi		Cosmetic

	DESCRIPTION:

		GetSecsMex is a variant of GetSecs which strips out the Psychtoolbox PsychScriptingGlue mex/mx abstraction layer.
		The purpsoe is to provide a simplified example to Mathworks and verify that the psychtoolbox mex/mx abstraction layer
		is not the source of trouble.  

	TARGET LOCATION:

		GetSecsMex.mexmac resides in:
			PsychToolbox/PsychObsolete/PsychBasicOSX/GetSecsMex.mexmac
			
		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/GetSecsMex_CopyOut.sh


*/


#include "mex.h"
#include <CoreAudio/HostTime.h>


	
