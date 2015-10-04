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


static char useString[] = "[status =] Eyelink('OpenFile', filename [, dontOpenExisting=0])";

static char synopsisString[] = 
    "Opens an EDF file 'filename' on the tracker computer, closes any existing file.\n"
    "If the optional flag 'dontOpenExisting' is set to a non-zero value, then the file "
    "is only opened, and thereby created, if it doesn't already exist. Otherwise the function "
    "aborts with an error.\n"
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
   int dontOpenExisting = 0;
   char *filename;

   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(2));
   PsychErrorExit(PsychRequireNumInputArgs(1));
   PsychErrorExit(PsychCapNumOutputArgs(1));

   // Verify eyelink is up and running
   EyelinkSystemIsConnected();
   EyelinkSystemIsInitialized();

   PsychAllocInCharArg(1, TRUE, &filename);

   // Should we only open new files, not open - and overwrite - existing ones?
   if (PsychCopyInIntegerArg(2, FALSE, &dontOpenExisting) && dontOpenExisting) {
       // Yes. Check if file already exists, abort with error if so:
       if (file_exists(filename) != 0) {
           printf("Eyelink openfile: The EDF file '%s' already exists and i was asked to abort in this case, so i'll abort.\n", filename);
           PsychErrorExitMsg(PsychError_user, "Tried to open already existing EDF file and user asked to not do that, so i abort.");
       }
   }

   iOpenFileStatus = open_data_file(filename);
   if (iOpenFileStatus!=0)
      mexPrintf("Eyelink openfile:  Cannot create EDF file '%s' errorcode : %d\n", filename, iOpenFileStatus);
 
   /* if there is an output variable available, assign iOpenFileStatus to it.   */			
   PsychCopyOutDoubleArg(1, FALSE, iOpenFileStatus);
   
   return(PsychError_none);
}
