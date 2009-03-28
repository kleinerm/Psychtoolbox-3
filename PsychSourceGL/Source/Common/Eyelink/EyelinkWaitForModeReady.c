/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkWaitForModeReady.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		15/06/06  fwc		Adapted from early alpha version by emp.
		27/03/09  edf       the help said the opposite of the API manual re: return value, fixed it
 
	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "[result = ] Eyelink('WaitForModeReady', maxwait)";

static char synopsisString[] =
"After a mode-change command is given to the EyeLink tracker,\n"
"an additional 5 to 30 milliseconds may be needed to complete mode setup.\n"
"Call this function after mode change functions to wait until they are finished setting up.\n"
"maxwait is in milliseconds -- max duration to wait for the mode to change.\n"
"Returns 0 if mode switching is done, else still waiting (assume a tracker error has occurred).\n";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKwaitformodeready
PURPOSE:
   uses INT16 CALLTYPE eyelink_wait_for_mode_ready(UINT32 maxwait);
   waits till new mode is finished setup
   maxwait = 0 to just test flag 
   rtns current state of switching flag*/

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
