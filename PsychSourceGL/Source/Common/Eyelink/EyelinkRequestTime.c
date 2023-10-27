/* EyeRequestTime.c */
/*
 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkRequestTime.c
 
	PROJECTS: Eyelink 
 
	AUTHORS:
 E.Peters@ai.rug.nl				emp
 f.w.cornelissen@med.rug.nl		fwc
 
	PLATFORMS:	Currently only OS X  
 
	HISTORY:
 
 28/06/06	fwc		adapted from alpha version
 
	TARGET LOCATION:
 
 Eyelink.mexmac resides in:
 PsychHardware/EyelinkToolbox/EyelinkBasic
 
 */

#include "PsychEyelink.h"

static char useString[] = "[status =] Eyelink('RequestTime')";

static char synopsisString[] = 
"sends request for tracker time update\n"
"returns: 0 if succesful, else link error code\n";

static char seeAlsoString[] = "";

/*
 ROUTINE: EyelinkRequestTime
 PURPOSE:
	sends request for tracker time update
	returns: 0 if no error, else link error code */

PsychError EyelinkRequestTime(void)
{
	int	iStatus			= -1;
	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){
		PsychGiveHelp();
		return(PsychError_none);
	};
	
	//check to see if the user supplied superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(0));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	
	iStatus = eyelink_request_time();
	
	PsychCopyOutDoubleArg(1, FALSE, iStatus);
	
	return(PsychError_none);		
}
