/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkCommand.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/23/05  cdb		Created.
		30-10-06  fwc		does now actually output status
		19-06-11  mk            Refactored to share parser with other functions.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

// Help strings
static char useString[] =  "[status =] Eyelink('Command', 'formatstring', [...])";

static char synopsisString[] = 
   "link command formatting."
   " use just like printf()."
   " Only chars and ints allowed in arguments!"
   " returns command result,"
   " allows 500 msec. for command to finish.";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKcommand
PURPOSE:
  uses int eyecmd_printf(char *fmt, ...)
  The Eyelink tracker accepts text commands through the link.
  These commands may be used to configure the system, open
  data file, and so on. This function is used with the same formatting
  methods as printf(), allowing numbers to be included in commands.
  The function waits up to 500 msec for a success or failure code to
  be returned from the tracker, then returns the error code NO_REPLY.
  RETURNS: 0 if OK, else errorcode. 
*/

PsychError EyelinkCommand(void)
{
	int iStatus = -1;

	// Add help strings
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}

	// Check arguments
	PsychErrorExit(PsychRequireNumInputArgs(1));
	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

	// Build eyelink command and execute
	iStatus = eyecmd_printf(PsychEyelinkParseToString(1));

	// Copy out the command result
	PsychCopyOutDoubleArg(1, FALSE, iStatus);
   
	return(PsychError_none);
}
