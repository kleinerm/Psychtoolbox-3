/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkSendKeyButton.c
  
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
*/

#include "PsychEyelink.h"

static char useString[] = "[result =] Eyelink('SendKeyButton', code, mods, state)";

static char synopsisString[] =
	"sends key/button state chane to tracker\n"
	"code = KB_BUTTON, mods = number for button\n"
	"state is KB_PRESS, KB_RELEASE, or KB_REPEAT\n"
	"Returns: OK_RESULT or LINK_TERMINATED_RESULT";

static char seeAlsoString[] = "";

/*
ROUTINE: EyelinkSendKeyButton
PURPOSE:
   uses INT16 CALLTYPE eyelink_send_keybutton(UINT16 code, UINT16 mods, INT16 state);
   "sends key/button state chane to tracker"
   "code = KB_BUTTON, mods = number for button"
   "state is KB_PRESS, KB_RELEASE, or KB_REPEAT"
   "Returns: OK_RESULT or LINK_TERMINATED_RESULT" */
   
PsychError EyelinkSendKeyButton(void)
{
//	char*	cCode = NULL;
	int		iCode		= 0;
	int		iMods		= 0;
	int		iResult		= 0;
	int		iState		= 0;

	// Add help strings
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}

	// Check arguments
	PsychErrorExit(PsychCapNumInputArgs(3));

	PsychErrorExit(PsychRequireNumInputArgs(3));

	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
//printf("EyelinkSendKeyButton: 1\n");

//	PsychAllocInCharArg(1, TRUE, &cCode);

//printf("EyelinkSendKeyButton: 2\n");
//	printf("EyelinkSendKeyButton: %d, %s\n",iCode, (char)iCode);
	PsychCopyInIntegerArg(1, TRUE, &iCode);
	PsychCopyInIntegerArg(2, TRUE, &iMods);
	PsychCopyInIntegerArg(3, TRUE, &iState);
	
	iResult = (int) eyelink_send_keybutton((UINT16) iCode, (UINT16) iMods, (INT16) iState);
//	iResult = eyelink_send_keybutton(cCode, iMods, iState);
	
	// Copy out arg
	PsychCopyOutDoubleArg(1, FALSE, iResult);
	
	return(PsychError_none);
	
}

