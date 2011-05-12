/*

 EyelinkTrackerMode.c		
 
 AUTHORS:
 
 nuha@sr-research.com nj
 
 PLATFORMS:
 
 All.
 
 
 HISTORY:
 
 mm/dd/yy
 
 17/02/11	nj		Created. 
 
 */

#include "EyelinkTrackerMode.h"
#include "PsychEyelink.h"
//#include "core_expt.h"
#include <stdlib.h>
#include <stdio.h>

# ifdef WIN32
#include <GLUT/glut.h>
#else
#include <GLUT/glut.h>
#endif

// Help strings
static char useString[] =  "[mode =] Eyelink('TrackerMode')";

static char synopsisString[] =
" Returns raw EyeLink mode numbers, defined in eyelink.h\n";

static char seeAlsoString[] = "";

PsychError EyelinkTrackerMode(void)
{
	int				mode = -1;
	
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
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	mode = eyelink_tracker_mode();
		
	PsychCopyOutDoubleArg(1, FALSE, mode);
	
	return(PsychError_none);
	
	
}