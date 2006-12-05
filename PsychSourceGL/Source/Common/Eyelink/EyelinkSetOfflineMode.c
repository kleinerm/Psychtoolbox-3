/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkSetOfflineMode.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		2001		emp/fwc		implemented OS9 version
		15/06/06	fwc			Adapted from early alpha version.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "Eyelink('SetOfflineMode')";

static char synopsisString[] = 
    "enter tracker idle mode, wait  till finished mode switch";

static char seeAlsoString[] = "";
 
/*
ROUTINE: EYELINKsetofflinemode
PURPOSE:
  uses void set_offline_mode(void)
  enter tracker idle mode, wait  till finished mode switch*/

PsychError EyelinkSetOfflineMode(void)
{
   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(0));
   PsychErrorExit(PsychRequireNumInputArgs(0));
   PsychErrorExit(PsychCapNumOutputArgs(0));

	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

   set_offline_mode();
   
   return(PsychError_none);
}
