/*
	PsychToolbox3/Source/OSX/Screen/PsychVideoGlue.h
	
	PLATFORMS:	
	
		This is the OS X Core Graphics version only.  
	
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		10/24/02		awi		Wrote it mostly by modifying SDL-specific refugees (from an experimental SDL-based Psychtoolbox).

							
	DESCRIPTION:
                
        Video Glue consists of PsychScreenGlue, PsychWindowGlue and PsychSurfaceGlue. 
	
		PsychVideoGlue.h is a platform-specific file which contains headers and types which abstract out 
        platform-specific calls for handling displays, windows and surfaces.  The Psychtoolbox uses the 
        term "video" to describe functions which interact with those elements of displays whereas the term
        "graphics" describes drawing functions, which with the Psychtoolbox are all implemented with 
        OpenGL calls and need not be abstracted to use across platforms.    							 

*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychVideoGlue
#define PSYCH_IS_INCLUDED_PsychVideoGlue

#include "Screen.h"

//define some constants
#define		kPsychMaxPossibleDisplays			10
#define		kPsychMaxNumberWindowBuffers		2		//The maximum upper bound.  Really it depends on display hardware and should be probed.
#define 	kPsychAllWindowsFull				TRUE	//If true then all onscreen windows must be full size.
#define		kPsychAllowMultipleOnscreenWindows	FALSE	//If true then we can have more than one onscreen window per display. 

//end include once
#endif

