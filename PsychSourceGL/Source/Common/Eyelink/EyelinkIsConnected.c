/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkIsConnected.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/21/05  cdb		Created.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

// Help strings
static char useString[] = "status = Eyelink('IsConnected')";

static char synopsisString[] = 
  "checks if the eyelink is (still) connected\n"
  "returns 1 if connected\n"
  "       -1 if dummy-connected\n" 
  "       2 if broadcast-connected\n" 
  "        0 if not connected\n";

static char seeAlsoString[] = "Initialize";

PsychError EyelinkIsConnected(void)
{
	int		iStatus		= -1;
	double	*pfOutArg	= NULL;
	
	// Add help strings
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}
	
	// Check arguments
	PsychErrorExit(PsychCapNumOutputArgs(1));
	PsychErrorExit(PsychCapNumInputArgs(0));
	
	iStatus = eyelink_is_connected();
	
    // Allocate an output matrix.  Even if argument is not there, we still get the space.     
    PsychAllocOutDoubleArg(1, FALSE, &pfOutArg);
	*pfOutArg = iStatus;
	
	return(PsychError_none);
}
   
