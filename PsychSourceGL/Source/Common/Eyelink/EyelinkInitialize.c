/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkInitialize.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/23/05  cdb		Created.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
			
	NOTES:
		This is a scaled down version compared to what Enno and Frans wrote.
		I hardcode in the buffersize and ignore other params.  And there is 
		no dummy version.
								--- cdb
*/

#include "PsychEyelink.h"

char useString[] = "[status =] Eyelink('Initialize')";

static char synopsisString[] = 
   "Initializes SIMLINK and Ethernet system\n"
   "Opens connection, report any problem\n"
   "Returns: 0 if OK, -1 if error";

static char seeAlsoString[] = "";

PsychError EyelinkInitialize(void)
{
	int		iBufferSize = 20000; // Hardcode default
	int		iStatus		= -1;

	// Add help strings
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}
	
	// Check arguments
	PsychErrorExit(PsychCapNumInputArgs(0));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	if(giSystemInitialized == 1) {
		// Is there a psych function for this?
		mexPrintf("Eyelink already initialized!\n");
		iStatus = 0;
	} else {
		iStatus = open_eyelink_system(iBufferSize, NULL);
		// If link is established, open connection
		if(iStatus != 0) {
			msec_delay(300);
			iStatus = eyelink_open();
		}
		
		// Check for errors and report
		if(iStatus != 0) {
			close_eyelink_system();
			if(iStatus == CONNECT_TIMEOUT_FAILED) {
				PsychErrorExitMsg(PsychError_user, "EyeLink: Initialize:  Cannot connect to EyeLink\n");
			} else if(iStatus == WRONG_LINK_VERSION) {
				PsychErrorExitMsg(PsychError_user, "EyeLink: Initialize:  Link version mismatch to EyeLink tracker\n");
			} else if(iStatus == LINK_INITIALIZE_FAILED) {
				PsychErrorExitMsg(PsychError_user, "EyeLink: Initialize:  Cannot initialize link\n");
			}
		} else {
			giSystemInitialized = 1;
		}
	}
	
	// Copy output arg
	PsychCopyOutDoubleArg(1, FALSE, iStatus);
	
	return(PsychError_none);
	
}