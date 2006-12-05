/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkAcceptTrigger.c
  
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


static char useString[] = "[result = ] Eyelink('AcceptTrigger')";

static char synopsisString[] = 
   "call to trigger acceptance of target fixation";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKaccepttrigger
PURPOSE:
   uses INT16 CALLTYPE eyelink_accept_trigger(void);
   call to trigger acceptance of target fixation */


PsychError EyelinkAcceptTrigger(void)
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

   result = eyelink_accept_trigger();

   /* if there is an output variable available, assign result to it.   */
   PsychCopyOutDoubleArg(1, FALSE, result);
   
   return(PsychError_none);
}
