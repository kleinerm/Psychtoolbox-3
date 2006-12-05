/*
	PsychVersioning.h		
    
	AUTHORS:
 
	Allen.Ingling@nyu.edu		  awi 
	mario.kleiner at tuebingen.mpg.de mk
	PLATFORMS:

		All
		

	HISTORY:
	
		3/13/03		awi		Created.
		11/3/04		awi		Incremented version number to 1.0.2 from 1.0.1 for 3 November release.
		11/18/04	awi		changed "Matlab" to "MATLAB"
		10/05/06        mk              Incremented version number to 3.0.8.
  
*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychVersioning
#define PSYCH_IS_INCLUDED_PsychVersioning

#include "Psych.h"

//these are all constants defined only for reporting versioning.  They are returned by Module('Version')
#define PSYCHTOOLBOX_MAJOR_VERSION				3
#define PSYCHTOOLBOX_MINOR_VERSION				0
#define PSYCHTOOLBOX_POINT_VERSION				8
//If you fork my source please change this name.  - awi 
#define PSYCHTOOLBOX_PROJECT_NAME				"OpenGL Psychtoolbox"
#if PSYCH_SYSTEM == PSYCH_OSX
//Placeholder for the OS X version at build time. 
#define PSYCHTOOLBOX_OS_NAME					"Apple OS X" 
#endif
#if PSYCH_SYSTEM == PSYCH_WINDOWS
//Placeholder for the M$-Windows version at build time.
#define PSYCHTOOLBOX_OS_NAME					"Microsoft Windows"  
#endif
#if PSYCH_SYSTEM == PSYCH_LINUX
//Placeholder for the GNU/Linux version at build time.
#define PSYCHTOOLBOX_OS_NAME                                    "GNU/Linux X11"
#endif

#if PSYCH_LANGUAGE == PSYCH_MATLAB
//Placeholder for the MATLAB version at build time.
#define PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME	                "Matlab"
#endif
#if PSYCH_LANGUAGE == PSYCH_OCTAVE
//Placeholder for the Octave version at build time.
#define PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME	                "GNU/Octave"
#endif
	
char *PsychGetBuildDate(void);
char *PsychGetBuildTime(void);
int PsychGetBuildNumber(void);
char *PsychGetVersionString(void);
int PsychGetMajorVersionNumber(void);
int PsychGetMinorVersionNumber(void);
int PsychGetPointVersionNumber(void);

//end include once
#endif
	

