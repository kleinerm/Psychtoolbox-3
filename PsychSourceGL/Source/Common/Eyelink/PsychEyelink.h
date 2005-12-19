/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/PsychEyelink.h
  
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

#ifndef PSYCH_IS_INCLUDED_Eyelink
#define PSYCH_IS_INCLUDED_Eyelink

// PsychToolbox Includes
#include "Psych.h"

// Eyelink Includes
#include "eyelink.h"


/////////////////////////////////////////////////////////////////////////
// Global variables used throughout eyelink C files

extern char		gstrCommand[32];
extern int		giSystemInitialized;


/////////////////////////////////////////////////////////////////////////
//		Eyelink Function prototypes

// Defined in PsychEyelink.c
PsychError	EyelinkSystemIsConnected(void);
PsychError	EyelinkSystemIsInitialized(void);

// Defined in EyelinkSynopsis.c
void		InitializeSynopsis();
PsychError	PsychDisplayEyelinkSynopsis(void);

// Defined in MODULEVersion.c
PsychError	MODULEVersion(void);

// Eyelink Target sub-commands
PsychError	EyelinkButtonStates(void);
PsychError	EyelinkCommand(void);
PsychError	EyelinkCurrentMode(void);
PsychError	EyelinkImageModeDisplay(void);
PsychError	EyelinkInitialize(void);
PsychError	EyelinkInitWindow(void);
PsychError	EyelinkIsConnected(void);
PsychError	EyelinkNewestFloatSample(void);
PsychError	EyelinkSendKeyButton(void);
PsychError	EyelinkShutdown(void);
PsychError	EyelinkStartRecording(void);
PsychError	EyelinkStartSetup(void);
PsychError	EyelinkStopRecording(void);

// PSYCH_IS_INCLUDED_Eyelink
#endif 
