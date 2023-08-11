/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkMessage.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	All.
    
	HISTORY:

		15/06/06  fwc		Adapted from early alpha version by emp.
		19-06-11  mk            Refactored to share parser with other functions.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "[status =] Eyelink('Message', 'formatstring', [...])";

static char synopsisString[] = 
   "link message formatting use just like printf()\n" 
   " Only chars and ints allowed in arguments!\n"
   "returns any send error";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKmessage
PURPOSE:
  uses int eyemsg_printf(char *fmt, ...)
  This sends a text message to the Eyelink tracker, which
  timestamps it and writes it to the EDF data file. Messages
  are useful for recording trail conditions, subject responses.
  or the time of important events. This function is used with
  the same formatting methods as printf(), allowing numbers to be
  included. Avoid"\n" at end of messages.
  RETURNS: 0 if OK, else errorcode. */
  	
PsychError EyelinkMessage(void)
{
   int status = -1;
   
   // All sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   // Check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychRequireNumInputArgs(1));
   PsychErrorExit(PsychCapNumOutputArgs(1));
   
   // Verify eyelink is up and running
   EyelinkSystemIsConnected();
   EyelinkSystemIsInitialized();
   
   status = eyemsg_printf(PsychEyelinkParseToString(1));

   /* if there is an output variable available, assign eyecmd_printf status to it.   */
   PsychCopyOutDoubleArg(1, FALSE, status);
   
   return(PsychError_none);
}
