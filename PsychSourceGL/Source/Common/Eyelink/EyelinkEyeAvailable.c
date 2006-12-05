/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkEyeAvailable.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

	12-05-2001	ep 		created it
	22-05-2001	fwc		added actual output values in synopsis. 
	22/06/06  fwc		Adapted for OSX from early alpha version by emp.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"


static char useString[] = "eyeused = Eyelink('EyeAvailable')";

static char synopsisString[]=
   "returns 0 (LEFT_EYE), 1 (RIGHT_EYE) or 2 (BINOCULAR) "
   "depending on what data is available "
   "returns -1 if none available."
   "ONLY VALID AFTER STARTSAMPLES EVENT READ";

static char seeAlsoString[] = "";

/*
#define LEFT_EYE  0   // index and ID of eyes 
#define RIGHT_EYE 1
#define LEFTEYEI  0
#define RIGHTEYEI 1
#define LEFT      0
#define RIGHT     1

#define BINOCULAR 2   // data for both eyes available

*/

/*
ROUTINE: EYELINKeyeavailable
PURPOSE:
   uses INT16 CALLTYPE eyelink_eye_available(void);
   returns LEFT_EYE, RIGHT_EYE or BINOCULAR
   depending on what data is available
   returns -1 if none available
   ONLY VALID AFTER STARTSAMPLES EVENT READ */

PsychError EyelinkEyeAvailable(void)
{
   int eyeused;
   
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

   eyeused = eyelink_eye_available();

   PsychCopyOutDoubleArg(1, TRUE, eyeused);
   
   return(PsychError_none);
}
