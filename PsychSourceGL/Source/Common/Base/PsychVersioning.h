/*
    PsychVersioning.h

    AUTHORS:

        Allen.Ingling@nyu.edu		  awi
        mario.kleiner.de@gmail.com        mk

    PLATFORMS:

        All

    HISTORY:

        3/13/03     awi     Created.
        11/3/04     awi     Incremented version number to 1.0.2 from 1.0.1 for 3 November release.
        11/18/04    awi     changed "Matlab" to "MATLAB"
        10/05/06    mk      Incremented version number to 3.0.8.
        04/11/11    mk      Incremented version number to 3.0.9 to mark license change and
                            broken backwards compatibility wrt. video capture etc.

*/

//begin include once
#ifndef PSYCH_IS_INCLUDED_PsychVersioning
#define PSYCH_IS_INCLUDED_PsychVersioning

#include "Psych.h"

//these are all constants defined only for reporting versioning.  They are returned by Module('Version')
#define PSYCHTOOLBOX_MAJOR_VERSION              3
#define PSYCHTOOLBOX_MINOR_VERSION              0
#define PSYCHTOOLBOX_POINT_VERSION              22
//If you fork my source please change this name.  - awi
#define PSYCHTOOLBOX_PROJECT_NAME               "OpenGL Psychtoolbox"
#if PSYCH_SYSTEM == PSYCH_OSX
//Placeholder for the OS X version at build time.
#define PSYCHTOOLBOX_OS_NAME                    "Apple macOS"
#endif
#if PSYCH_SYSTEM == PSYCH_WINDOWS
//Placeholder for the M$-Windows version at build time.
#define PSYCHTOOLBOX_OS_NAME                    "Microsoft Windows"
#endif
#if PSYCH_SYSTEM == PSYCH_LINUX
//Placeholder for the GNU/Linux version at build time.
#ifdef PTB_USE_WAYLAND
#define PSYCHTOOLBOX_OS_NAME                                    "GNU/Linux WAYLAND"
#else
#ifdef PTB_USE_WAFFLE
#define PSYCHTOOLBOX_OS_NAME                                    "GNU/Linux WAFFLE"
#else
#define PSYCHTOOLBOX_OS_NAME                                    "GNU/Linux X11"
#endif
#endif
#endif

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
  #define PTB_ISA "Intel"
#else
  #if defined(__arm__) || defined(_M_ARM) || defined(__aarch64__) || defined(_M_ARM64)
    #define PTB_ISA "ARM"
  #else
    #define PTB_ISA "unknown arch"
  #endif
#endif

#if defined(__LP64__) || defined(_WIN64)
#define PTB_ARCHITECTURE "64-Bit"
#else
#define PTB_ARCHITECTURE "32-Bit"
#endif

#if PSYCH_LANGUAGE == PSYCH_MATLAB
#ifndef PTBOCTAVE3MEX
// Binary plugin for MATLAB:
#define PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME                "Matlab"
#else
// Binary plugin for Octave-3 or later via the MEX interface:
#define PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME                "GNU/Octave"
#endif
#endif

#if PSYCH_LANGUAGE == PSYCH_PYTHON
// Binary plugin for Python:
#define PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME                "Python"
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
