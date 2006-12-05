/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkCurrentMode.c
  
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

static char useString[] = "mode = Eyelink('CurrentMode')";

static char synopsisString[] =
   "returns current tracker state as bit flags"
   " -1 = disconnect, 0 = unknown";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKcurrentmode
PURPOSE:
   uses INT16 CALLTYPE eyelink_current_mode(void);
   returns current tracker state as bit flags
   -1 = disconnect, 0 = unknown
   */

PsychError EyelinkCurrentMode(void)
{
	int		iMode	= 0;
	
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
	
	iMode = eyelink_current_mode();
	
	// Assign output arg
	PsychCopyOutDoubleArg(1, TRUE, iMode);
	
	return(PsychError_none);
}
