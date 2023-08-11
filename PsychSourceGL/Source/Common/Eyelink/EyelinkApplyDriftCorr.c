/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkApplyDriftCorr.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		2002	emp		created OS9 version
		15/06/06	fwc		Adapted from early alpha version by emp.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"


static char useString[] = "[result = ] Eyelink('ApplyDriftCorr')";

static char synopsisString[] = 
   "apply correction from last drift correction";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKapplydriftcorr
PURPOSE:
   uses INT16 CALLTYPE eyelink_apply_driftcorr(void);
   apply correction from last drift correction*/


PsychError EyelinkApplyDriftCorr(void)
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

   result = eyelink_apply_driftcorr();
   
   /* if there is an output variable available, assign result to it.   */
   PsychCopyOutDoubleArg(1, FALSE, result);
   
   return(PsychError_none);
}
