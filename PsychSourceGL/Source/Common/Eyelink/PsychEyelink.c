/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/PsychEyelink.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/22/05  cdb		Created.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

/////////////////////////////////////////////////////////////////////////
// Global variables used throughout eyelink C files

char	gstrCommand[32];
int		giSystemInitialized = 0;


/////////////////////////////////////////////////////////////////////////
// Check is system is initialized
//
PsychError EyelinkSystemIsConnected(void)
{
	if (!eyelink_is_connected()) {
		PsychErrorExitMsg(PsychError_user, "Eyelink system is not connected!\n");
	}
}

/////////////////////////////////////////////////////////////////////////
// Check is system is initialized
//
PsychError EyelinkSystemIsInitialized(void)
{
	if (giSystemInitialized != 1) {
		PsychErrorExitMsg(PsychError_user, "Eyelink system is not initialized!\n");
	}
}

