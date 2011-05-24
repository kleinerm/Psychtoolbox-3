/*
 PsychSourceGL/Source/Common/Eyelink/EyelinkReadFromTracker.c

 PLATFORMS: All
 
 PROJECTS: Eyelink 
 
 AUTHORS:

 nuha@sr-research.com nj
  
 HISTORY:
 
 11-03-09  nj		created
 
*/

#include "PsychEyelink.h"

static char useString[] = "[result, reply] = Eyelink('ReadFromTracker', VariableName);";

static char synopsisString[] =
	"Sends a text variable 'VariableName' name whose value is to be read and returned by the tracker as a text string.\n"
	"Returns text with reply to last read request.\n";

static char seeAlsoString[] = "";

/*
 ROUTINE: EyelinkReadFromTracker 
 PURPOSE:
 Sends a text variable name whose value is to be read and returned by the tracker as a text string.
 Returns text with reply to last read request. */

PsychError EyelinkReadFromTracker(void)
{
	int		result;
	char	*varbuf;
	char    replybuf[100] = "";
	int		t;
	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
		
	//check to see if the user supplied superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(1));
	PsychErrorExit(PsychRequireNumInputArgs(1));
	PsychErrorExit(PsychCapNumOutputArgs(2));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	PsychAllocInCharArg(1, TRUE, &varbuf);
	
	result = eyelink_read_request(varbuf);
	
	if (!result){
		t = current_msec();

		// Waits for a maximum of 500 msec
		while(current_msec()-t < 500)
		{
			if(eyelink_read_reply(replybuf) == OK_RESULT)
			{
				break;
			}
            
            // OS friendly wait for at least 1 msec before retry:
            PsychYieldIntervalSeconds(0.001);
		}
	}

	PsychCopyOutDoubleArg(1, TRUE, result);
	PsychCopyOutCharArg(2, TRUE, replybuf);	

	return(PsychError_none);
}
