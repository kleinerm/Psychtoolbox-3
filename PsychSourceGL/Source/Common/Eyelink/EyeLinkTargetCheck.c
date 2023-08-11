/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkTargetCheck.c
  
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

static char useString[] = "[result, tx, ty] = Eyelink('TargetCheck')";

static char synopsisString[] =
   "gets pixel X, Y of target\n"
   "returns 1 if visible, 0 if not";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKtargetcheck
PURPOSE:
   uses INT16 CALLTYPE eyelink_target_check(INT16 FARTYPE *x, INT16 FARTYPE *y);
   gets pixel X, Y of target
   returns 1 if visible, 0 if not*/

PsychError EyelinkTargetCheck(void)
{
   INT16 tx;
   INT16 ty;
   int result;

   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(0));
   PsychErrorExit(PsychRequireNumInputArgs(0));
   PsychErrorExit(PsychCapNumOutputArgs(3));
 
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
             
   result = eyelink_target_check(&tx, &ty);

   PsychCopyOutDoubleArg(1, TRUE, result);
   PsychCopyOutDoubleArg(2, TRUE, (int)tx);
   PsychCopyOutDoubleArg(3, TRUE, (int)ty);
   
   return(PsychError_none);
}
