/* EyelinkDriftCorrStart.c */

/* 
	/osxptb/trunk/PsychSourceGL/Source/Common/Eyelink/EyelinkDriftCorrStart.c
  
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

static char useString[] = "[status = ] Eyelink('DriftCorrStart', x, y [,dtype=0][, dodraw=1][, allow_setup=0])";

static char synopsisString[] = 
   "Start drift correction, specify target position with x and y.\n"
   "If the optional 'dtype' argument is zero, the routine eyelink_driftcorr_start() "
   "is called. If 'dtype' is one, do_drift_correct() is called. In that case, the "
   "argument 'dodraw' selects if Eyelink itself should draw the target (1), or "
   "if it is left to usercode. 'allow_setup' if set to 1, will allow eyelink to "
   "enter the setup menu if ESCape key is pressed. Otherwise, eyelink will "
   "terminate a running drift correction on press of the ESCape key.\n";

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
   int dtype, dodraw, allow_setup;
   
   // Defaults for optional arguments:
   dtype = 0;
   dodraw = 1;
   allow_setup = 0;
   
   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(5));
   PsychErrorExit(PsychRequireNumInputArgs(2));
   PsychErrorExit(PsychCapNumOutputArgs(1));

    // Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

   PsychCopyInIntegerArg(1, TRUE, &x);
   PsychCopyInIntegerArg(2, TRUE, &y);
   
   // Get optional flags:
   PsychCopyInIntegerArg(3, FALSE, &dtype);
   PsychCopyInIntegerArg(4, FALSE, &dodraw);
   PsychCopyInIntegerArg(5, FALSE, &allow_setup);
   
   if (dtype == 0) {
       status = (int) eyelink_driftcorr_start((INT16) x,(INT16) y);
   }
   else {
       status = (int) do_drift_correct((INT16) x,(INT16) y, (INT16) dodraw, (INT16) allow_setup);
   }
   
   /* if there is an output variable available, assign openstatus to it.   */
   PsychCopyOutDoubleArg(1, FALSE, status);
   
   return(PsychError_none);
}
