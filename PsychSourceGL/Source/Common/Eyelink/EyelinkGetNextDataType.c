/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkGetNextDataType.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		29/05/2001	emp 		created it (OS9 version)
		30/10/06	fwc			Adapted from early alpha version.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			PsychHardware/EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "type = Eyelink('GetNextDataType')";

static char synopsisString[] = 
   " returns item type of next queue item"
   " SAMPLE_TYPE if sample, 0 if none, else event code";

static char seeAlsoString[] = "";
 
/*
ROUTINE: EyelinkGetNextDataType
PURPOSE:
   uses INT16 CALLTYPE eyelink_get_next_data(NULL);
   just returns item type  of next queue item
   SAMPLE_TYPE if sample, 0 if none, else event code*/

PsychError EyelinkGetNextDataType(void)
{
   int type = 0;

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

   type = eyelink_get_next_data(NULL);

   PsychCopyOutDoubleArg(1, TRUE,  type);
   
   return(PsychError_none);
}
