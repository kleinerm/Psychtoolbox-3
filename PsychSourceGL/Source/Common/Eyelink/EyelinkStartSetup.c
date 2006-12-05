/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkStartSetup.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/23/05  cdb		Created.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "[result =] Eyelink('StartSetup')";

static char synopsisString[] = "Enters setup mode ";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKstartsetup
PURPOSE:
   uses INT16 CALLTYPE eyelink_start_setup(void);
   enters setup mode */

PsychError EyelinkStartSetup(void)
{
	int	iResult = 0;

	// Add help strings
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}

	// Check arguments
	PsychErrorExit(PsychCapNumInputArgs(0));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

	iResult = eyelink_start_setup();

	// Copy out result
	PsychCopyOutDoubleArg(1, FALSE, iResult);
   
	return(PsychError_none);

}
