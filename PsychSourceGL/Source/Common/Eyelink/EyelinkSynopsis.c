/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkSynopsis.c
  
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

#define MAX_SYNOPSIS_STRINGS 500  

//declare variables local to this file.  
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

void InitializeSynopsis()
{
	int i=0;
	const char **synopsis = synopsisSYNOPSIS;  //abbreviate the long name

	synopsis[i++] = "Usage:";

	// Init or close eyelink
	synopsis[i++] = "\n% Initialize or shutdown Eyelink connection:";
	synopsis[i++] = "[status =] Eyelink('Initialize')";
	synopsis[i++] = "status = Eyelink('IsConnected')";
	synopsis[i++] = "Eyelink('Shutdown')";

	// Calibration
	synopsis[i++] = "\n% Calibration:";
	synopsis[i++] = "[result =] Eyelink('StartSetup')";
	synopsis[i++] = "Eyelink('InitWindow')";
	synopsis[i++] = "[result =] Eyelink('ImageModeDisplay')";
	synopsis[i++] = "mode = Eyelink('CurrentMode')";
	
	// Start or stop recording, Data acquisition
	synopsis[i++] = "\n% Start or stop recording and acquiring data:";
	synopsis[i++] = "[startrecording_error =] Eyelink('StartRecording' [,file_samples, file_events, link_samples, link_events] )";
	synopsis[i++] = "Eyelink('Stoprecording')";
	synopsis[i++] = "sample = Eyelink('NewestFloatSample')";

	// Misc eyelink communication:
	synopsis[i++] = "\n% Miscellaneous functions to communicate with Eyelink:";
	synopsis[i++] = "result = Eyelink('ButtonStates')";
	synopsis[i++] = "[status =] Eyelink('Command', 'formatstring', [...])";
	synopsis[i++] = "[result =] Eyelink('SendKeyButton', code, mods, state)";
	
		
	// Place Holder
	/*
	synopsis[i++] = "\n% Minimal port of Eyelink toolbox to OS X";
	synopsis[i++] = "A small subset of functions currently defined.";
	synopsis[i++] = "Chris Burns needs to fill this info in further.";
	synopsis[i++] = "The original EyelinkToolbox was developed by:";
	synopsis[i++] = "\tFrans Cornelissen";
	synopsis[i++] = "\tEnno Peters";
	synopsis[i++] = "\tJohn Palmer";
	synopsis[i++] = "Chris Burns developing OS X port only.";
	*/
	
	synopsis[i++] = NULL;  //this tells PsychDisplayScreenSynopsis where to stop
	if (i > MAX_SYNOPSIS_STRINGS) {
		PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)MAX_SYNOPSIS_STRINGS,(long)i);
	}
}

PsychError PsychDisplayEyelinkSynopsis(void)
{
	int i;
	
	for (i = 0; synopsisSYNOPSIS[i] != NULL; i++) {
		printf("%s\n", synopsisSYNOPSIS[i]);
	}
		
	return(PsychError_none);
}
