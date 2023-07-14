/*
 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkTrackerTime.c
 
	PROJECTS: Eyelink 
 
	AUTHORS:
 f.w.cornelissen@rug.nl		fwc
 
	PLATFORMS:	OSX, windows
 
	HISTORY:
 
 21/01/07	fwc		added based on EyelinkReadTime function
 
	TARGET LOCATION:
 
 Eyelink.mexmac resides in:
 PsychHardware/EyelinkToolbox/EyelinkBasic
 
 */

#include "PsychEyelink.h"

static char useString[] = "[time =] Eyelink('TrackerTime')";
	
static char synopsisString[] = 
  "Returns the current tracker time (in seconds)\n"
  "since the tracker application started.\n";

static char seeAlsoString[] = "";
   
/*
ROUTINE: EyelinkTrackerTime
PURPOSE:
	Returns the current tracker time (in seconds)
	since the tracker application started, uses usec precision.
	We use the eyelink_tracker_double_usec() function, and scale to seconds, keeping precision
 */
          
PsychError EyelinkTrackerTime(void)
{
   double time;

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

   time = eyelink_tracker_double_usec()/1000000;
   
   PsychCopyOutDoubleArg(1, FALSE, time);
   
   return(PsychError_none);	
}
