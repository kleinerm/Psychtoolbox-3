/*
  Psychtoolbox2/Source/Common/PsychIncludes.h		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All 
  
  PROJECTS:
  07/16/02	awi		Screen on MacOS9
   

  HISTORY:
  07/16/02  awi		Pulled out of PsychPlatform.h
  11/15/02  awi		Added includes for OSX.  
  
  DESCRIPTION:
  PsychIncludes.h includes all C, system, and language binding 
  header files which a Psychtoolbox library 
  would require.
  
  This file should ONLY be included by PsychConstants.h 

  T0 DO: 
	  

*/

#include "PsychPlatform.h"

//Includes dependent on runtime environment:
#if PSYCH_LANGUAGE == PSYCH_MATLAB
    // Include File with Matlab interface API definitions:
    #include "mex.h"
#endif

// Platform independent include for glew: This is a catch-all
// for all OpenGL definitions and functions, currently up to
// OpenGL 2.1:
#include "../Screen/glew.h"

//platform dependent includes stage 1
#if PSYCH_SYSTEM == PSYCH_LINUX
	//These are not needed anymore with GLEW:
	//#include <GL/gl.h>
	//#include <GL/glu.h>
	//#include <GL/glx.h>
	//#include <GL/glext.h>
	
	// This is the new glew include for GLX extension support:
	#include "../Screen/glxew.h"
	
        #include <X11/Xlib.h>
        #include <X11/keysym.h>
        #include <sys/time.h>
        #include <unistd.h>
        #include <stdarg.h>
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
	#include <windows.h>
	//#include <gl/gl.h>
	//#include <gl/glu.h>
	#include "../Screen/wglew.h"

#elif PSYCH_SYSTEM == PSYCH_OS9
	#include <Types.h>
	#include <MacTypes.h>
	#include <Events.h>
	#include <stdarg.h>
	#include <string.h>
	#include <ctype.h>
	#include "gl.h"
	#include "glu.h"
#elif PSYCH_SYSTEM == PSYCH_OSX
	//includes for Apple OpenGL
	#include <sys/time.h>

        // If we are included from PsychScriptingGlue.cc, which
        // is indicated by PTBINSCRIPTINGGLUE, then we must only
        // include MacTypes.h, not the rest of Carbon, ApplicationServices
        // et al. -> Some of the Octave header files conflict with Apple
        // system header files --> do not include the Apple headers when
        // building Scriptingglue - they are not needed but would prevent
        // the build.
        #ifndef PTBINSCRIPTINGGLUE
        // File included during compile of some PTB file other than ScriptingGlue...
        #include <Carbon/Carbon.h>
        #include <CoreServices/CoreServices.h>
        #include <ApplicationServices/ApplicationServices.h>
        #include <CoreAudio/HostTime.h>
        #else
        // File included from ScriptingGlue - only import minimal amount of headers...
        #include <CoreServices/../Frameworks/CarbonCore.framework/Headers/MacTypes.h>
        #endif

	#include <OpenGL/OpenGL.h>
	//#include <OpenGL/gl.h>
	//#include <OpenGL/glext.h>
	//#include <OpenGL/glu.h>
#endif 

//C standard library headers
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <float.h>

#if PSYCH_LANGUAGE == PSYCH_OCTAVE
// File with pseudo MATLAB interface API definitions:
// Emulates missing Matlab functions and structures...
// Stored in the Source/Octave subfolder...
    #include <octavemex.h>
#endif

//SDL_getenv.h includes a macro which redefines getenv() to be
//SDL getenv().   This then conflicts with the statement "using 
//std::getenv" within stdlib.h.  To avoid the conflict, include 
//SDL.h only after the standard includes above. 
//platform dependent includes stage 2
#if PSYCH_SYSTEM == PSYCH_WINDOWS

#elif PSYCH_SYSTEM == PSYCH_OS9
	#include "SDL.h"
#endif

