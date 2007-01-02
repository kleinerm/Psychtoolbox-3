/*
	ScreenPreferenceState.c		

	AUTHORS:
		
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:	
	
		Only OS X for now.

	HISTORY:
	
		2/28/04  awi		Created.   
		5/30/05  mk         New preference setting screenSkipSyncTests.
		5/30/05  mk         New preference setting screenVisualDebugLevel.
		3/7/06   awi        Added state for new preference flag SuppressAllWarnings. 


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
int  PsychPrefStateGet_TextYPositionIsBaseline(void);
void PsychPrefStateSet_TextYPositionIsBaseline(int textPosIsBaseline);

void PsychPrefStateGet_DefaultFontName(const char **fontName);
void PsychPrefStateSet_DefaultFontName(const char *newName);

int PsychPrefStateGet_DefaultTextSize(void);
void PsychPrefStateSet_DefaultTextSize(int textSize);

int PsychPrefStateGet_DefaultTextStyle(void);
void PsychPrefStateSet_DefaultTextStyle(int textStyle);

Boolean PsychPrefStateGet_TextAlphaBlending(void);
void PsychPrefStateSet_TextAlphaBlending(Boolean enableFlag);

// Screen self-test and calibration preferences.  
int PsychPrefStateGet_SkipSyncTests(void);
void PsychPrefStateSet_SkipSyncTests(int level);

int PsychPrefStateGet_VisualDebugLevel(void);
void PsychPrefStateSet_VisualDebugLevel(int level);

int PsychPrefStateGet_ConserveVRAM(void);
void PsychPrefStateSet_ConserveVRAM(int level);

Boolean PsychPrefStateGet_EmulateOldPTB(void);
void PsychPrefStateSet_EmulateOldPTB(Boolean level);

// 3D graphics rendering support:
Boolean PsychPrefStateGet_3DGfx(void);
void PsychPrefStateSet_3DGfx(Boolean level);

//Debug preferences.  
Boolean PsychPrefStateGet_DebugMakeTexture(void);
void PsychPrefStateSet_DebugMakeTexture(Boolean setFlag);

// Master switch for debug output:
Boolean PsychPrefStateGet_SuppressAllWarnings(void);
void PsychPrefStateSet_SuppressAllWarnings(Boolean setFlag);

// Level of verbosity:
int PsychPrefStateGet_Verbosity(void);
void PsychPrefStateSet_Verbosity(int level);

// Master control for method of flip and vbl timestamping:
int PsychPrefStateGet_VBLTimestampingMode(void);
void PsychPrefStateSet_VBLTimestampingMode(int level);


//end include once
#endif


