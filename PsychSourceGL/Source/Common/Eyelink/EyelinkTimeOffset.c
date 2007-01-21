/*
 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkTimeOffset.c
 
	PROJECTS: Eyelink 
 
	AUTHORS:
 f.w.cornelissen@rug.nl		fwc
 
	PLATFORMS:	OSX, windows
 
	HISTORY:
 
 21/01/07	fwc		added based on EyelinkTrackerTime function
 
	TARGET LOCATION:
 
 Eyelink.mexmac resides in:
 PsychHardware/EyelinkToolbox/EyelinkBasic
 
 */

#include "PsychEyelink.h"

static char useString[] = "[offset =] Eyelink('TimeOffset')";
	
static char synopsisString[] = 
  "Returns the time difference (in msec)\n"
  "between the tracker time and display pc time.\n";

static char seeAlsoString[] = "";
   
/*
ROUTINE: EyelinkTimeOffset
PURPOSE:
	Returns the time difference between the tracker time and display pc time.
	eyelink_double_usec_offset() returns current offset between local and tracker time in microseconds.
	We scale it to mseconds, while keeping precision
    May "jiggle" by 50 usec or more.
 */
          
PsychError EyelinkTimeOffset(void)
{
   double offset;

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

   offset = eyelink_double_usec_offset()/1000;
   
   PsychCopyOutDoubleArg(1, FALSE, offset);
   
   return(PsychError_none);	
}
