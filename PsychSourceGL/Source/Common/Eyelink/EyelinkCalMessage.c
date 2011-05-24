/* 
 /osxptb/trunk/PsychSourceGL/Source/Common/Eyelink/EyelinkCalMessage.c
 
 PROJECTS: Eyelink 
 
 AUTHORS:
 nuha@sr-research.com nj

 HISTORY:
 
 11-03-04  nj		created

*/

#include "PsychEyelink.h"

static char useString[] = "[result, messageString] = Eyelink('CalMessage');";

static char synopsisString[] =
"Returns 'messageString' text associated with result of last calibration, "
"validation, or drift correction. This usually specifies "
"errors or other statistics.";

static char seeAlsoString[] = "";

/*
 ROUTINE: EYELINKcalmessage 
 PURPOSE:
 Returns text associated with result of last calibration, validation, or drift correction. This usually specifies
 errors or other statistics.
*/

PsychError EyelinkCalMessage(void)
{
	int		result;
	char	strMessage[256];

	// Clear strings
	memset(strMessage, 0, sizeof(strMessage));
		
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check to see if the user supplied superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(0));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(2));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	result = eyelink_cal_message(strMessage);

	PsychCopyOutDoubleArg(1, TRUE, result);
	PsychCopyOutCharArg(2, TRUE, strMessage);	

	return(PsychError_none);
}
