/*
	ScreenPreferenceState.c		

	AUTHORS:
		
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:	
	
		Only OS X for now.

	HISTORY:
	
		2/28/04  awi		Created.   
                5/30/05  mk             New preference setting screenSkipSyncTests.
                5/30/05  mk             New preference setting screenVisualDebugLevel.


	DESCRIPTION:

	
		This file holds state for the Screen Preference command


*/


#include "Screen.h"

//begin include once 
#ifndef PSYCH_IS_INCLUDED_ScreenPreferenceState
#define PSYCH_IS_INCLUDED_ScreenPreferenceState




//function prototypes


void PrepareScreenPreferences(void);

// Text matching.  Function definition in MiniBox.c 
Boolean PsychIsPsychMatchCaseSensitive(void);

//PsychTable preferences
int PsychPrefStateGet_PsychTableVersion(void);
const char *PsychPrefStateGet_PsychTableCreator(void);

//Text and Font preferences
void PsychPrefStateGet_DefaultFontName(const char **fontName);
void PsychPrefStateSet_DefaultFontName(const char *newName);

int PsychPrefStateGet_DefaultTextSize(void);
void PsychPrefStateSet_DefaultTextSize(int textSize);

int PsychPrefStateGet_DefaultTextStyle(void);
void PsychPrefStateSet_DefaultTextStyle(int textStyle);

Boolean PsychPrefStateGet_TextAlphaBlending(void);
void PsychPrefStateSet_TextAlphaBlending(Boolean enableFlag);

// Screen self-test and calibration preferences.  
Boolean PsychPrefStateGet_SkipSyncTests(void);
void PsychPrefStateSet_SkipSyncTests(Boolean setFlag);

int PsychPrefStateGet_VisualDebugLevel(void);
void PsychPrefStateSet_VisualDebugLevel(int level);

//Debug preferences.  
Boolean PsychPrefStateGet_DebugMakeTexture(void);
void PsychPrefStateSet_DebugMakeTexture(Boolean setFlag);

//end include once
#endif

