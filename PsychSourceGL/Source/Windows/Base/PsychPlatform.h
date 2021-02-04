/*
    PsychPlatform.h

    PLATFORMS:

        Only Windows

    AUTHORS:

        Mario Kleiner   mk  mario.kleiner.de@gmail.com

    HISTORY:

    DESCRIPTION:

        PsychPlatform.h contains constant definitions asserting conditions
        specific to the the OS X version of Screen.  If you
        use a conditional macro wich branches according to one of the constants
        in this file, then that conditional macro does not belong here.

        Note that this should be the ONLY Psychtoolbox file which is conditionally
        included by platform.  Exceptions to this rule might be:
        -project.def files included in VC++ files
        -StdAfx pre-compiled header files included in VC++ files.

    TO DO:

        For now the project path specifies which platform version of this file to
        include, with each version of this file defining constants which identify
        the platform.  A smarter way to do this would be to use a single version
        of this file which detects contants defined within the compilation
        environment, for example as a flag passed to the compiler.
*/

#ifndef PSYCH_PLATFORM_WIN32_H
#define PSYCH_PLATFORM_WIN32_H

#include "PsychPlatformConstants.h"

//these control build switches
#define PSYCH_SYSTEM        PSYCH_WINDOWS

#ifndef PSYCH_LANGUAGE
#define PSYCH_LANGUAGE      PSYCH_MATLAB
#endif

#define PSYCH_DEBUG         PSYCH_ON

// Only needed on GNU/Octave + MinGW64, as of Octave-6.1. MSVC 2019 for Python
// and Matlab builds already defines a sufficiently high WINVER:
#ifdef PTBOCTAVE3MEX
    // Need to define _WIN32_WINNT and WINVER as 0x0602, so we can use features
    // added in Windows-8 and in the Win-8 SDK. This obviously needs at least
    // Windows-8 as build- and runtime system, but as we only officially support
    // Windows-10, this is not a problem.
    // #warning Manually setting WINVER to 0x0602
    #undef _WIN32_WINNT
    #undef WINVER
    #define _WIN32_WINNT 0x0602
    #define WINVER       0x0602
#endif

// PSYCH_PLATFORM_WIN32_H
#endif
