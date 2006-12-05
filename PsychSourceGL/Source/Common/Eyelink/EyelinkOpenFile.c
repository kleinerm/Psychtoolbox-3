/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkOpenFile.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		18/10/02	fwc		removed superfluous message when succesfully opening file
		15/06/06	fwc		Adapted from early alpha version by emp.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"


static char useString[] = "[status =] Eyelink('OpenFile','filename')";

static char synopsisString[] = 
    "Opens an EDF file, closes any existing file\n"
    "Returns 0 if success, else error code" ;

static char seeAlsoString[] = "";
 
/*
ROUTINE: EyelinkOpenFile
PURPOSE:
  uses INT16 open_data_file(char *name); 
  Opens an EDF file on tracker hard disk, closes any existing file
  Returns 0 if success, else error code */

PsychError EyelinkOpenFile(void)
{
   int iOpenFileStatus = -1;
   char *filename;

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

   PsychAllocInCharArg(1, TRUE, &filename);

   iOpenFileStatus = open_data_file(filename);
   if (iOpenFileStatus!=0)
      mexPrintf("Eyelink openfile:  Cannot create EDF file '%s' errorcode : %d\n", filename, iOpenFileStatus);
 
   /* if there is an output variable available, assign iOpenFileStatus to it.   */			
   PsychCopyOutDoubleArg(1, FALSE, iOpenFileStatus);
   
   return(PsychError_none);
}
