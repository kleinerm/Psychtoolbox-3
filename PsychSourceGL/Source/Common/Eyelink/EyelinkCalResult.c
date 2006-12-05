/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkCalResult.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		15/06/06  fwc		Adapted from early alpha version by emp.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"


static char useString[] = "result = Eyelink('CalResult')";

static char synopsisString[] =
	"check if drift correction, calibration done."
	" Returns OK_RESULT, ABORT_RESULT, NO_REPLY."
	" Reading result resets it to NO_REPLY.";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKcalresult
PURPOSE:
    uses INT16 CALLTYPE eyelink_cal_result(void);
	check if drift correction, calibration done
	returns OK_RESULT, ABORT_RESULT, NO_REPLY 
	Reading result resets it to NO_REPLY */


PsychError EyelinkCalResult(void)
{
   int result;
   
   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(0));
   PsychErrorExit(PsychRequireNumInputArgs(0));
   PsychErrorExit(PsychCapNumOutputArgs(1));
 
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

   result = eyelink_cal_result();
   
   PsychCopyOutDoubleArg(1, TRUE, result);
   
   return(PsychError_none);
}
