/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkStopRecording.c
  
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

static char useString[] = "Eyelink('Stoprecording')";

static char synopsisString[] = "Stop recording eye data (stop_recording).";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKstoprecording
PURPOSE:
  uses void stop_recording(void);
  halt recording, return when tracker finished mode switch
*/

PsychError EyelinkStopRecording(void)
{
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
	PsychErrorExit(PsychCapNumOutputArgs(0));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	//EyelinkSystemIsInitialized();

	stop_recording();
	
	return(PsychError_none); 
}
