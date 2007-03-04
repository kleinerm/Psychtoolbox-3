/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkSynopsis.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@rug.nl		fwc

  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/21/05  cdb		Created based on old EyeMex.c and ScreenSynopsis.c.
		15/06/06  fwc		Added few functions and small changes to synopsis.
		

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

	synopsis[i++] = "\n% This is main function of the Eyelink toolbox";
	synopsis[i++] = "Usage:";
   synopsis[i++] = "\n% For general advice, try:";
   synopsis[i++] = "help eyelink";
   synopsis[i++] = "\n% For a more detailed explanation of any eyelink function, just add a question mark \"?\".";
   synopsis[i++] = "% E.g. for 'Initialize',try either of these equivalent forms:";
   synopsis[i++] = "eyelink('Initialize?')";
   synopsis[i++] = "eyelink Initialize?";
   synopsis[i++] = "% If you think you've found a bug,\n";
   synopsis[i++] = "Please report on the forum, see: http://psychtoolbox.org/\n";

	// Init or close eyelink
	synopsis[i++] = "\n% Initialize or shutdown Eyelink connection:";
	synopsis[i++] = "[status =] Eyelink('Initialize')";
	synopsis[i++] = "[status =] Eyelink('InitializeDummy')";
	synopsis[i++] = "status = Eyelink('IsConnected')";
	synopsis[i++] = "Eyelink('Shutdown')";
	synopsis[i++] = "[status =] Eyelink('OpenFile','filename')";
	synopsis[i++] = "[status =] Eyelink('CloseFile')";
	synopsis[i++] = "[status =] Eyelink('ReceiveFile',['filename'], ['dest'], ['dest_is_path'])";
	
	// Calibration
	synopsis[i++] = "\n% Calibration:";
	synopsis[i++] = "[result =] Eyelink('StartSetup')";
	synopsis[i++] = "[status = ] Eyelink('DriftCorrStart', x, y)";
	synopsis[i++] = "[result = ] Eyelink('ApplyDriftCorr')";
	synopsis[i++] = "[result, tx, ty] = Eyelink('TargetCheck')";
	synopsis[i++] = "[result = ] Eyelink('AcceptTrigger')";
	
	// Start or stop recording, Data acquisition
	synopsis[i++] = "\n% Start or stop recording and acquiring data:";
	synopsis[i++] = "[startrecording_error =] Eyelink('StartRecording' [,file_samples, file_events, link_samples, link_events] )";
	synopsis[i++] = "Eyelink('Stoprecording')";
	synopsis[i++] = "error = Eyelink('CheckRecording')";
	synopsis[i++] =  "eyeused = Eyelink('EyeAvailable')";
	synopsis[i++] = "NewOrOld = Eyelink('NewFloatSampleAvailable')";
	synopsis[i++] = "sample = Eyelink('NewestFloatSample')";
	synopsis[i++] = "[sample, raw] = Eyelink('NewestFloatSampleRaw')";
	synopsis[i++] = "type = Eyelink('GetNextDataType')";
	synopsis[i++]  = "item = Eyelink('GetFloatData', type)";

	// Misc eyelink communication:
	synopsis[i++] = "\n% Miscellaneous functions to communicate with Eyelink:";
	synopsis[i++] = "result = Eyelink('ButtonStates')";
	synopsis[i++] = "[status =] Eyelink('Command', 'formatstring', [...])";
	synopsis[i++] = "[status =] Eyelink('Message', 'formatstring', [...])";
	synopsis[i++] = "[result =] Eyelink('SendKeyButton', code, mods, state)";
	synopsis[i++] = "[time =] Eyelink('TrackerTime')";
	synopsis[i++] = "[offset =] Eyelink('TimeOffset')";
	synopsis[i++] = "[status =] Eyelink('RequestTime')";
	synopsis[i++] = "[time =] Eyelink('ReadTime')";

	synopsis[i++] = "\n% Miscellaneous Eyelink functions:";
	synopsis[i++] = "[result =] Eyelink('WaitForModeReady', maxwait)";
	synopsis[i++] = "[result =] Eyelink('ImageModeDisplay')";
	synopsis[i++] = "mode = Eyelink('CurrentMode')";
	synopsis[i++] = "result = Eyelink('CalResult')";
	synopsis[i++] = "Eyelink('SetOfflineMode')";
	synopsis[i++] = "[version, versionString]  = Eyelink('GetTrackerVersion')";
	synopsis[i++] = "[time =] Eyelink('TrackerTime')";
	synopsis[i++] = "[offset =] Eyelink('TimeOffset')";

	// Place Holder
	synopsis[i++] = "\n\n\n\n% EyelinkToolbox version for the OpenGL PsychToolbox";
	synopsis[i++] = "% The EyelinkToolbox was developed by:";
	synopsis[i++] = "\tFrans Cornelissen";
	synopsis[i++] = "\tEnno Peters";
	synopsis[i++] = "\tJohn Palmer";
	synopsis[i++] = "\tChris Burns";
	synopsis[i++] = "\tMario Kleiner";
	
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

