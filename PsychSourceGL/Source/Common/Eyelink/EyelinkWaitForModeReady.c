/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkWaitForModeReady.c
  
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

static char useString[] = "[result = ] Eyelink('WaitForModeReady', maxwait)";

static char synopsisString[] =
   "waits till new mode is finished setup\n"
   "maxwait = 0 to just test flag\n"
   "returns current state of switching flag: 0 if not ready";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKwaitformodeready
PURPOSE:
   uses INT16 CALLTYPE eyelink_wait_for_mode_ready(UINT32 maxwait);
   waits till new mode is finished setup
   maxwait = 0 to just test flag 
   rtns current state of switching flag: 0 if not ready*/

PsychError EyelinkWaitForModeReady(void)
{
   int maxwait;
   int result = -1;
   
   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(1));
   PsychErrorExit(PsychRequireNumInputArgs(1));
   PsychErrorExit(PsychCapNumOutputArgs(1));
 
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

   PsychCopyInIntegerArg(1, TRUE, &maxwait);      
   
   result = eyelink_wait_for_mode_ready(maxwait);

   /* if there is an output variable available, assign result to it.   */
   PsychCopyOutDoubleArg(1, FALSE, result);
   
   return(PsychError_none);
}
