/*
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkCheckRecording.c
  
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

static char useString[] = "error = Eyelink('CheckRecording')";

static char synopsisString[] =
   " Check if we are recording: if not, report an error."
   " Also calls record_abort_hide() if recording aborted."
   " Returns 0 if recording in progress."
   " Returns ABORT_EXPT if link disconnected."
   " Handles recors abort menu if trial interrupted."
   " Returns TRIAL_ERROR if other non-recording state.";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKcheckrecording
PURPOSE:
   uses INT16 check_recording(void);
   Check if we are recording: if not, report an error
 Not:  Also calls record_abort_hide() if recording aborted
   Returns 0 if recording in progress 
   Returns ABORT_EXPT if link disconnected 
   Handles record abort menu if trial interrupted
   Returns TRIAL_ERROR if other non-recording state
   Typical use is  
     if((error=check_recording())!=0) return error; */


PsychError EyelinkCheckRecording(void)
{
   int error;
   
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

   error = check_recording();

   PsychCopyOutDoubleArg(1, TRUE, error);
   
   return(PsychError_none);
}

