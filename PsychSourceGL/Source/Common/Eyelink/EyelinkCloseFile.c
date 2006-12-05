/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkCloseFile.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		2002		emp/fwc		implemented
		15/06/06	fwc			Adapted from early alpha version.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"


static char useString[] = "[status =] Eyelink('CloseFile')";

static char synopsisString[] = 
    "Closes EDF file on tracker hard disk."
	" Returns 0 if success, else error code.";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKclosefile
PURPOSE:
  uses INT16 close_data_file(void); 
  Closes EDF file on tracker hard disk
  Returns 0 if success, else error code */

PsychError EyelinkCloseFile(void)
{
   int closefilestatus = -1;

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

   closefilestatus = close_data_file();
   
   if (closefilestatus != 0)
      printf("Eyelink closefile: Error in closing file %d\n", closefilestatus);

   /* if there is an output variable available, assign closefilestatus to it.   */
   PsychCopyOutDoubleArg(1, FALSE, closefilestatus);
   
   return(PsychError_none);
}
