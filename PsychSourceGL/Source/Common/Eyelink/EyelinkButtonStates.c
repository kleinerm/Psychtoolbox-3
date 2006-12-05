/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkButtonStates.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/22/05  cdb		Created.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

// Help strings
static char useString[] = "result = Eyelink('ButtonStates')";

static char synopsisString[] = 
   "reads the currently-known state of all buttons\n"
   "the bits in the returned value will be\n"
   "set (1) if corresponding button is pressed\n"
   "LSB is button 1, MSB is button 16\n"
   "currently only 8 buttons available\n";

static char seeAlsoString[] = "SendKeyButton";

PsychError EyelinkButtonStates(void)
{
	UInt16	iButtons	= 0;
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
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	iButtons = eyelink_button_states();
	
    // Allocate an output matrix.  Even if argument is not there, we still get the space.     
    PsychAllocOutDoubleArg(1, FALSE, &pfOutArg);
	*pfOutArg = iButtons;
	
	return(PsychError_none);
}
   
