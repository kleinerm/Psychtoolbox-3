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
		29/06/06	fwc		fixed EyelinkSystemIsConnected to allow dummy mode connections

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

/////////////////////////////////////////////////////////////////////////
// Global variables used throughout eyelink C files

int		giSystemInitialized = 0;


/////////////////////////////////////////////////////////////////////////
// Check if system is initialized
//
PsychError EyelinkSystemIsConnected(void)
{
	int iStatus=-9999;
	iStatus=eyelink_is_connected();
//	mexPrintf("EyelinkSystemIsConnected status %d ((iStatus==0)=%d)\n", iStatus, (iStatus==0) );
	if (iStatus==0) {
		PsychErrorExitMsg(PsychError_user, "Eyelink system is not connected!\n");
	}
/*	
	if (eyelink_is_connected()==0) {
		PsychErrorExitMsg(PsychError_user, "Eyelink system is not connected!\n");
	}
	*/
	return(PsychError_none);
}

/////////////////////////////////////////////////////////////////////////
// Check is system is initialized
//
PsychError EyelinkSystemIsInitialized(void)
{
	if (giSystemInitialized != 1) {
		PsychErrorExitMsg(PsychError_user, "Eyelink system is not initialized!\n");
	}
	return(PsychError_none);
}


