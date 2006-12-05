/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkStartRecording.c
  
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

static char useString[] = "[startrecording_error =] Eyelink('StartRecording' [,file_samples, file_events, link_samples, link_events] )";

static char synopsisString[] = "Start recording with data types requested";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKstartrecording
PURPOSE:
  uses INT16 start_recording(INT16 file_samples, INT16 file_events,
		   			  INT16 link_samples, INT16 link_events);

  Start recording with data types requested
  Check that all requested link data is arriving
  return 0 if OK, else trial exit code
  Start data recording to EDF file, BEFORE DISPLAY.
  You should always start recording 50-100 msec before required
  otherwise you may lose a few msec of data */	   			  

PsychError EyelinkStartRecording(void)
{
	int		iFileEvents		= 1;
	int		iFileSamples	= 1;
	int		iLinkEvents		= 1;
	int		iLinkSamples	= 1;
	int		iStatus			= -1;
	char	strMsg[256];
	
	memset(strMsg, 0, sizeof(strMsg));
	
	// Add help strings
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}

	// Check arguments
	PsychErrorExit(PsychCapNumInputArgs(4));
	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

	// Check if default values are overridden
	PsychCopyInIntegerArg(1, FALSE, &iFileSamples);
	PsychCopyInIntegerArg(2, FALSE, &iFileEvents);
	PsychCopyInIntegerArg(3, FALSE, &iLinkSamples);
	PsychCopyInIntegerArg(4, FALSE, &iLinkEvents);
	
	iStatus = (int) start_recording((INT16) iFileSamples,(INT16) iFileEvents,(INT16) iLinkSamples,(INT16) iLinkEvents);
	if (iStatus != 0) {
		// BUG?  Recording starts and appears to be working, but does not
		// return 0 and aborts if we use the PsychErrorExit...
		mexPrintf("Eyelink startrecording error %d\n", iStatus);
		//sprintf(strMsg, "Eyelink startrecording error %d\n", iStatus);
		//PsychErrorExitMsg(PsychError_internal, strMsg);
	}
	
	// Copy out result
	PsychCopyOutDoubleArg(1, FALSE, iStatus);
   
	return(PsychError_none);
}

