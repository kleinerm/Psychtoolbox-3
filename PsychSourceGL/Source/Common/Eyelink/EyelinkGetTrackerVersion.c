/*
 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkGetTrackerVersion.c
 
	PROJECTS: Eyelink 
 
	AUTHORS:
 E.Peters@ai.rug.nl				emp
 f.w.cornelissen@med.rug.nl		fwc
 
	PLATFORMS:	Currently only OS X  
 
	HISTORY:
 
 28/06/06	fwc		created
 
	TARGET LOCATION:
 
 Eyelink.mexmac resides in:
 PsychHardware/EyelinkToolbox/EyelinkBasic
 
 */

#include "PsychEyelink.h"

static char useString[] = "[version , versionString]  = Eyelink('GetTrackerVersion')";

static char synopsisString[] =
"Returns: 0 if not connected, 1 for EyeLink I, 2 for EyeLink II\n"
"<Versionstring> will be 'EYELINK I' or 'EYELINK II x.xx', where 'x.xx' is the software version \n";

static char seeAlsoString[] = "";

/*
 ROUTINE: EyelinkGetTrackerVersion
 PURPOSE:
	checks for reply to eyelink_request_time()
	returns: 0 if no reply, else time
 */

PsychError EyelinkGetTrackerVersion(void)
{
	int iVersion=0;
	char strVersion[40]="unknown tracker type";
	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check to see if the user supplied superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(0));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(2));
	
//	// Verify eyelink is up and running
//	EyelinkSystemIsConnected();
//	EyelinkSystemIsInitialized();
	
	iVersion = eyelink_get_tracker_version(strVersion);
	
	//   mexPrintf("Tracker Version: '%s'\n", strVersion );
	PsychCopyOutDoubleArg(1, TRUE, iVersion);
	PsychCopyOutCharArg(2, FALSE, strVersion);
	
	return(PsychError_none);	
}
