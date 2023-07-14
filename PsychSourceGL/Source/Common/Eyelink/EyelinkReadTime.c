/*
 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkReadTime.c
 
	PROJECTS: Eyelink 
 
	AUTHORS:
 E.Peters@ai.rug.nl				emp
 f.w.cornelissen@rug.nl		fwc
 
	PLATFORMS:	Currently only OS X  
 
	HISTORY:
 
 28/06/06	fwc		adapted from alpha version
 
	TARGET LOCATION:
 
 Eyelink.mexmac resides in:
 PsychHardware/EyelinkToolbox/EyelinkBasic
 
 */

#include "PsychEyelink.h"

static char useString[] = "[time =] Eyelink('ReadTime')";
	
static char synopsisString[] = 
  "checks for reply to eyelink_request_time()\n"
  "returns: 0 if no reply, else time \n";

static char seeAlsoString[] = "";
   
/*
ROUTINE: EyelinkReadTime
PURPOSE:
	checks for reply to eyelink_request_time()
	returns: 0 if no reply, else time
 */
          
PsychError EyelinkReadTime(void)
{
   UINT32 time;

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

   time = eyelink_read_time();
   
   PsychCopyOutDoubleArg(1, FALSE, time);
   
   return(PsychError_none);	
}
