/* EyelinkDriftCorrStart.c */

/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkDriftCorrStart.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		12-05-2001	emp 		created OS9 version
		15-05-2001	fwc			fixed small error (tested prhs[1] and prhs[2]). 
		15/06/06	fwc			Adapted for OSX from early alpha version.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "[status = ] Eyelink('DriftCorrStart', x, y)";

static char synopsisString[] = 
   "Start drift correction, specify target position with x and y.";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKdriftcorrstart
PURPOSE:
   uses INT16 CALLTYPE eyelink_driftcorr_start(INT16 x, INT16 y); 
   start drift correction, specify target position */


PsychError EyelinkDriftCorrStart(void)
{
   int x, y;
   int status;
   
   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(2));
   PsychErrorExit(PsychRequireNumInputArgs(2));
   PsychErrorExit(PsychCapNumOutputArgs(1));

	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

   PsychCopyInIntegerArg(1, TRUE, &x);
   PsychCopyInIntegerArg(2, TRUE, &y);   
   
   status = (int) eyelink_driftcorr_start((INT16) x,(INT16) y);
   
   /* if there is an output variable available, assign openstatus to it.   */
   PsychCopyOutDoubleArg(1, FALSE, status);
   
   return(PsychError_none);
}
