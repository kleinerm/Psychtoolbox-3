/*
	PsychToolbox3/Source/Common/PsychSound/RegisterProject.h		

	PROJECTS: PsychSound only.  

	AUTHORS:

                mario.kleiner at tuebingen.mpg.de   mk
 
	PLATFORMS:	OS X only for now, but should work on all platforms.  

	HISTORY:

		10/14/05	mk		Created 


	DESCRIPTION: 
 
                Implementation of the PsychSound function, which is supposed to be a
                replacement for the old PTB SND() function.
 
                Design goals:
                - More flexibility, more functionality than old SND function.
                - Fully asynchronous operation - choice between sync/async mode.
                - Better timing, aka lowe latencies in start/stop/processing of sound playback or recording.
                - Bugfree(TM), as opposed to the very buggy Matlab drivers on Windows and MacOS-X.
                - Fully portable due to the use of OpenAL as underlying sound library (Available for OS-X, Windows, Linux, Unixes, ...)
 
	TARGET LOCATION:

		PsychSound.mexmac resides in:
			PsychToolbox/PsychBasic/PsychSound.mexmac
			
		To change the target location modify the script:
			Psychtoolbox/Tools/Scripts/CopyOutMexFiles/PsychSound_CopyOut.sh


*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_RegisterProject
#define PSYCH_IS_INCLUDED_RegisterProject

#include "Psych.h"
#include "PsychSound.h"


PsychError PsychModuleInit(void);

//end include once
#endif




