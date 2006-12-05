/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkImageModeDisplay.c
  
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

static char useString[] = "[result =] Eyelink('ImageModeDisplay')";

static char synopsisString[] =
	"This handles display of the EyeLink camera images. "
	"While in imaging mode, it contiuously requests "
	"and displays the current camera image. "
	"It also displays the camera name and threshold setting. "
	"Keys on the subject PC keyboard are sent to the tracker "
	"so the experimenter can use it during setup. "
	"It will exit when the tracker leaves "
	"imaging mode or discannects. "
    "RETURNS: 0 if OK, 32767 (Ox7FFF or TERMINATE_KEY) if pressed, -1 if disconnect";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKimagemodedisplay
PURPOSE:
   uses INT16 image_mode_display(void);

	This handles display of the EyeLink camera images
	While in imaging mode, it contiuously requests
	and displays the current camera image
	It also displays the camera name and threshold setting
	Keys on the subject PC keyboard are sent to the tracker
	so the experimenter can use it during setup.
	It will exit when the tracker leaves
	imaging mode or discannects

    RETURNS: 0 if OK, TERMINATE_KEY if pressed, -1 if disconnect*/

PsychError EyelinkImageModeDisplay(void)
{
	int		iResult		= 0;
	
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
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	if(0)
	{
		// NOTE:  Enno's OSX version added this eyelink call. Not sure if we want it or not.
		eyelink_start_setup();	
		iResult = image_mode_display();
	}
	else
	{
		//mexPrintf("EyelinkImageModeDisplay is not yet implemented.\n");
		iResult=0;
	}
	
	// Assign output arg if available
	PsychCopyOutDoubleArg(1, FALSE, iResult);
	
	return(PsychError_none);
}
