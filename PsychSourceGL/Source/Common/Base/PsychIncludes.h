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

//platform dependent includes stage 1
#if PSYCH_SYSTEM == PSYCH_LINUX
	#include "mex.h"
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glx.h>
	#include <GL/glext.h>
        #include <X11/Xlib.h>
        #include <X11/keysym.h>
        #include <sys/time.h>
        #include <unistd.h>
        #include <stdarg.h>
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
	#include "mex.h"
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>
        #include <Movies.h>
        #include <CoreVideo.h>
        #include <GXMath.h>
        #include <ImageCompression.h>

#elif PSYCH_SYSTEM == PSYCH_OS9
	#include <Types.h>
	#include <MacTypes.h>
	#include "mex.h"
	#include <Events.h>
	#include <stdarg.h>
	#include <string.h>
	#include <ctype.h>
	#include "gl.h"
	#include "glu.h"
#elif PSYCH_SYSTEM == PSYCH_OSX
	//includes for Matlab API
	#include "mex.h"
	//includes for Apple OpenGL
	#include <sys/time.h>
	#include <Carbon/Carbon.h>
	#include <ApplicationServices/ApplicationServices.h>
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
	#include <CoreAudio/HostTime.h>
	#include <CoreServices/CoreServices.h>
#endif 

//C standard library headers
#include <math.h>
#include <stdio.h>
#include <limits.h>

//SDL_getenv.h includes a macro which redefines getenv() to be
//SDL getenv().   This then conflicts with the statement "using 
//std::getenv" within stdlib.h.  To avoid the conflict, include 
//SDL.h only after the standard includes above. 
//platform dependent includes stage 2
#if PSYCH_SYSTEM == PSYCH_WINDOWS

#elif PSYCH_SYSTEM == PSYCH_OS9
	#include "SDL.h"
#endif 

 


