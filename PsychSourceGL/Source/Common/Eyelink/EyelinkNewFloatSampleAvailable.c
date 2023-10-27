/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkNewFloatSampleAvailable.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		24/05/2001	emp 		created it (OS9 version)
		15/06/06	fwc			Adapted from early alpha version.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "NewOrOld = Eyelink('NewFloatSampleAvailable')";

static char synopsisString[] =
   "checks if new (float) sample is available\n"
   "returns -1 if none or error, 0 if old, 1 if new";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKnewfloatsampleavailable
PURPOSE:
   uses INT16 CALLTYPE eyelink_newest_float_sample(void FARTYPE *buf);
   returns -1 if none or error, 0 if old, 1 if new */

PsychError EyelinkNewFloatSampleAvailable(void)
{
   int result = -1;

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

   result = eyelink_newest_float_sample(NULL);

   PsychCopyOutDoubleArg(1, TRUE, result);
   
   return(PsychError_none);
}
