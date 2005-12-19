/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkNewestFloatSample.c
  
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

static char useString[] = "sample = Eyelink('NewestFloatSample')";

static char synopsisString[] =
		"makes copy of most recent float sample received";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKnewestfloatsample
PURPOSE:
   uses INT16 CALLTYPE eyelink_newest_float_sample(void FARTYPE *buf);
   makes copy of most recent float sample received*/   

PsychError EyelinkNewestFloatSample(void)
{
	FSAMPLE		structFloatSample;
	mxArray		**mxOutArg;
	
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

	// Grab the sample
	eyelink_newest_float_sample(&structFloatSample);
	mxOutArg = PsychGetOutArgMxPtr(1);
	*mxOutArg = (mxArray *)CreateMXFSample(&structFloatSample);
	
	return(PsychError_none);
}
