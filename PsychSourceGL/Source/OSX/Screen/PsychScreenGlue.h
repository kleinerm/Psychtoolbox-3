/*
	PsychToolbox3/Source/OSX/Screen/PsychScreenGlue.h
	
	PLATFORMS:	
	
		This is the OS X Core Graphics version only.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		10/20/02		awi			Wrote it mostly by adding in SDL-specific refugeess (from an experimental SDL Psychtoolbox) from header and source files.
		11/16/04		awi			added  PsychGetGlobalScreenRect.  Enhanced DESCRIPTION text.  

        							
	DESCRIPTION:
	
		Functions in this file comprise an abstraction layer for probing and controlling screen state.  
		
		Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes() 
		should be platform-neutral, despite that the calls in OS X and Windows to detect available pixel sizes are
		different.  Platform specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, 
		PsychWindowTextClue.
	
		In addition to glue functions for windows and screen there are functions which implement shared functionality between between Screen commands,
		such as ScreenTypes.c and WindowBank.c. 
			
	NOTES:
	
	TO DO: 
	
		¥ The "glue" files should should be suffixed with a platform name.  The original (bad) plan was to distingish platform-specific files with the same 
		name by their placement in a directory tree.
		
		¥ All of the functions which accept a screen number should be suffixed with "...FromScreenNumber". 
 
		
     							
*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychScreenGlue
#define PSYCH_IS_INCLUDED_PsychScreenGlue

#include "Screen.h"


//functions from PsychScreenGlue
void						InitializePsychDisplayGlue(void);
void						PsychGetCGDisplayIDFromScreenNumber(CGDirectDisplayID *displayID, int screenNumber);
void						PsychCaptureScreen(int screenNumber);
void						PsychReleaseScreen(int screenNumber);
boolean						PsychIsScreenCaptured(int screenNumber);
int						PsychGetNumDisplays(void);
void						PsychGetScreenDepths(int screenNumber, PsychDepthType *depths);
boolean						PsychCheckVideoSettings(PsychScreenSettingsType *setting);
void						PsychGetScreenDepth(int screenNumber, PsychDepthType *depth);   //dont' use this and get rid  of it.
int						PsychGetScreenDepthValue(int screenNumber);
int						PsychGetNumScreenPlanes(int screenNumber);
float						PsychGetNominalFramerate(int screenNumber);
void						PsychGetScreenSize(int screenNumber, long *width, long *height);
void						PsychGetGlobalScreenRect(int screenNumber, double *rect);
void						PsychGetScreenRect(int screenNumber, double *rect);
void						PsychGetDisplaySize(int screenNumber, int *width, int *height);
PsychColorModeType                              PsychGetScreenMode(int screenNumber);
int						PsychGetDacBitsFromDisplay(int screenNumber);		//from display, not from preferences
void						PsychGetScreenSettings(int screenNumber, PsychScreenSettingsType *settings);
boolean						PsychSetScreenSettings(boolean cacheSettings, PsychScreenSettingsType *settings);
boolean						PsychRestoreScreenSettings(int screenNumber);
void						PsychHideCursor(int screenNumber);
void						PsychShowCursor(int screenNumber);
void						PsychPositionCursor(int screenNumber, int x, int y);
void						PsychReadNormalizedGammaTable(int screenNumber, int *numEntries, float **redTable, float **greenTable, float **blueTable);
void						PsychLoadNormalizedGammaTable(int screenNumber, int numEntries, float *redTable, float *greenTable, float *blueTable);


//end include once
#endif

