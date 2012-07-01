/*
        Psychtoolbox3/Source/Common/PsychConstants.h            
  
        AUTHORS:
 
        Allen.Ingling@nyu.edu                 awi 
        mario.kleiner@tuebingen.mpg.de        mk
 
        PLATFORMS:      All

        HISTORY:
        7/16/02         awi     Split off from PsychProject.h
  
        PROJECTS
        7/16/02         awi     MacOS9 Screen.  

        DESCRIPTION:
        PsychConstants.h defines constants which abstract out platform-specific types.  Note
        That all such abstract types are prefixed with "psych_" as in "psych_uint32".   

        This file also includes platform-specific macros.
*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychConstants
#define PSYCH_IS_INCLUDED_PsychConstants

#define __STDC_LIMIT_MACROS 1

//bring in the standard c and system headers 
#include "PsychIncludes.h"

#if PSYCH_LANGUAGE == PSYCH_MATLAB
	#undef printf
	#define printf mexPrintf
#endif

//platform dependent macro defines 
#if PSYCH_SYSTEM == PSYCH_WINDOWS
    // Define snprintf as _snprintf to take Microsoft brain-damage into account:
	#ifndef snprintf
	#define snprintf _snprintf
	#endif
#elif PSYCH_SYSTEM == PSYCH_OSX
#elif PSYCH_SYSTEM == PSYCH_LINUX
#endif 

#ifndef FALSE
#define FALSE   0
#define TRUE    1
#endif

#ifndef false
#define false FALSE
#endif

#ifndef true
#define true TRUE
#endif

// MK: These GL defines are likely obsolete - covered by glew.h
#ifndef GL_TABLE_TOO_LARGE
#define GL_TABLE_TOO_LARGE   0x8031  
#endif
#ifndef GL_TEXTURE_RECTANGLE_EXT
#define GL_TEXTURE_RECTANGLE_EXT 0x84F5
#endif
#ifndef GL_TEXTURE_RECTANGLE_NV
#define GL_TEXTURE_RECTANGLE_NV  0x84F5
#endif
#ifndef GL_UNSIGNED_INT_8_8_8_8
#define GL_UNSIGNED_INT_8_8_8_8  0x8035
#endif
// Define GL_BGRA as GL_BGRA_EXT aka 0x80E1 if it isn''t defined already
#ifndef GL_BGRA
#define GL_BGRA              0x80E1
#endif
#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_SAMPLES_ARB
#define GL_SAMPLES_ARB   0x80A9
#endif
#ifndef GL_SAMPLE_BUFFERS_ARB
#define GL_SAMPLE_BUFFERS_ARB   0x80A8
#endif
#ifndef GL_TEXTURE_DEPTH
#define GL_TEXTURE_DEPTH 0x8071
#endif

#ifndef PTBOCTAVE3MEX
// mwSize is a new type introduced around Matlab R2006b. Define
// it to be an integer on older Matlab releases and other runtime
// environments: (MX_API_VER is defined in Matlabs matrix.h file)
#ifndef MX_API_VER
	// Old Matlab without Mex API version definition:
	#define SELFMADE_MWSIZE 1
#else
	// Matlab with Mex API prior V7.3 (Release 2006b)?
	#if  MX_API_VER < 0x07030000
		#define SELFMADE_MWSIZE 1
	#endif
#endif
#endif

// Do we need to define mwSize ourselves?
#ifdef SELFMADE_MWSIZE
	typedef int mwSize;
	typedef int mwIndex;
#endif

// Define our own base psych_bool type psych_bool to be an unsigned char,
// i.e., 1 byte per value on each platform:
typedef unsigned char		psych_bool;

//abstract up simple data types. 
#if PSYCH_SYSTEM == PSYCH_LINUX
        typedef int64_t                         psych_int64;
        typedef unsigned long long              psych_uint64;
        typedef unsigned int                    psych_uint32;
        typedef unsigned char                   psych_uint8;
        typedef unsigned short                  psych_uint16;
        typedef char                            Str255[256];

        // We don't have Quicktime for Linux, so we provide a little hack to
        // make the compiler happy:
        typedef void*                           CVOpenGLTextureRef;
        typedef int CGDisplayCount;
        // CGDirectDisplayID is typedef'd to a X11 display connection handle:
        typedef Display* CGDirectDisplayID;
        typedef int CGDisplayErr;
        typedef unsigned int CFDictionaryRef;
		
		// Datatype for condition variables:
		typedef pthread_cond_t		psych_condition;		
		// Datatype for Mutex Locks:
		typedef pthread_mutex_t		psych_mutex;
		// Datatype for threads:
		typedef pthread_t			psych_thread;
		typedef pthread_t			psych_threadid;		
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
        typedef LONGLONG						psych_int64;
        typedef ULONGLONG                       psych_uint64;
        typedef DWORD                           psych_uint32;
        typedef BYTE                            psych_uint8;
        typedef WORD                            psych_uint16;

        // The Microsoft Visual C compiler doesn't know about the __func__ keyword, but it knows __FUNCTION__ instead:
        #ifdef _MSC_VER
        #define __func__ __FUNCTION__
        #endif

        // Hack to make compiler happy until QT7 Windows supports this:
        typedef void* CVOpenGLTextureRef;
        typedef int CGDisplayCount;
        typedef HDC CGDirectDisplayID;
        typedef int CGDisplayErr;

		// Datatype for condition variables:
		typedef HANDLE					psych_condition;		
		// Datatype for Mutex Locks:
		typedef CRITICAL_SECTION		psych_mutex;
		// Datatype for threads:
		typedef struct psych_threadstruct {
			HANDLE				handle;			// Handle to actual thread (NULL == Invalid).
			DWORD				threadId;		// Unique numeric id (0 = Invalid.)
			psych_condition		terminateReq;	// Condition/Event object to signal the request for termination.
			HANDLE				taskHandleMMCS; // Handle to task for MMCSS scheduled thread, NULL otherwise.
		} psych_threadstruct;
		typedef struct psych_threadstruct*	psych_thread;

		typedef psych_uint32		psych_threadid;

#elif PSYCH_SYSTEM == PSYCH_OSX
        typedef UInt8				psych_uint8;
		typedef UInt16				psych_uint16;
        typedef UInt32				psych_uint32;
        typedef unsigned long long	psych_uint64;
        typedef long long			psych_int64;

		// Datatype for condition variables:
		typedef pthread_cond_t		psych_condition;
		// Datatype for Mutex Locks:	
		typedef pthread_mutex_t		psych_mutex;
		// Datatype for threads:
		typedef pthread_t			psych_thread;
		typedef pthread_t			psych_threadid;
#endif

#if PSYCH_LANGUAGE == PSYCH_MATLAB
        typedef const mxArray CONSTmxArray;
        #define PsychGenericScriptType mxArray
        typedef mxLogical PsychNativeBooleanType; 
#endif

#define EXP
#ifndef APIENTRY
#define APIENTRY
#endif

//end include once
#endif
