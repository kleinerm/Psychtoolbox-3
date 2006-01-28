/*
	PsychVersioning.h		
    
	AUTHORS:
 
	Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:

		Only OS X for now.
		

	HISTORY:
	
		3/13/03		awi		Created.
		11/3/04		awi		Incremented version number to 1.0.2 from 1.0.1 for 3 November release.
		11/18/04	awi		changed "Matlab" to "MATLAB"
  
*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychVersioning
#define PSYCH_IS_INCLUDED_PsychVersioning

#include "Psych.h"

//these are all constants defined only for reporting versioning.  They are returned by Module('Version')
#define PSYCHTOOLBOX_MAJOR_VERSION				1
#define PSYCHTOOLBOX_MINOR_VERSION				0
#define PSYCHTOOLBOX_POINT_VERSION				6
#define PSYCHTOOLBOX_PROJECT_NAME				"OpenGL Psychtoolbox"       //If you fork my source please change this name.  - awi 
#if PSYCH_SYSTEM == PSYCH_OSX
#define PSYCHTOOLBOX_OS_NAME					"Apple OS X"		    //Placeholder for the OS X version at build time.  
#endif
#if PSYCH_SYSTEM == PSYCH_WINDOWS
#define PSYCHTOOLBOX_OS_NAME					"Microsoft Windows"         //Placeholder for the M$-Windows version at build time.  
#endif
#define PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME	"MATLAB"				    //Placeholder for the MATLAB version at build time.

	
char *PsychGetBuildDate(void);
char *PsychGetBuildTime(void);
int PsychGetBuildNumber(void);
char *PsychGetVersionString(void);
int PsychGetMajorVersionNumber(void);
int PsychGetMinorVersionNumber(void);
int PsychGetPointVersionNumber(void);

//end include once
#endif
	