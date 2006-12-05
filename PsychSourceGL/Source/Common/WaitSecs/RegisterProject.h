/*
	PsychToolbox3/Source/Common/WaitSecs/RegisterProject.h		

	PROJECTS: WaitSecs only.  

	AUTHORS:

		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:	OS X only for now.  

	HISTORY:

		8/20/02		awi		Created. 
		1/20/02		awi		Created derived the GetSecs version from the Screen version.  
		7/07/04		awi		Cosmetic


	DESCRIPTION: 

	TARGET LOCATION:

		TestPriorityMex.mexmac resides in:
			PsychToolbox/PsychBasic/WaitSecs.mexmac
			
		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/WaitSecs_CopyOut.sh


*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_RegisterProject
#define PSYCH_IS_INCLUDED_RegisterProject

#include "Psych.h"
#include "WaitSecs.h"


PsychError PsychModuleInit(void);

//end include once
#endif




