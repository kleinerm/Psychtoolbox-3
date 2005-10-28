/*
	Psychtoolbox2/Source/Common/PsychConstants.h		
  
  	AUTHORS:
  	Allen.Ingling@nyu.edu		awi 
  
  	PLATFORMS:	Mac OS 9   

  	HISTORY:
  	7/16/02		awi	Split off from PsychProject.h
  
  	PROJECTS
  	7/16/02		awi	MacOS9 Screen.  

	DESCRIPTION:
	PsychConstants.h defines constants which abstract out platform-specific types.  Note
	That all such abstract types are prefixed with "psych_" as in "psych_uint32".   

	This file also includes platform-specific macros
	
	
	 
 

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychConstants
#define PSYCH_IS_INCLUDED_PsychConstants

//bring in the standard c and system headers 
#include "PsychIncludes.h"

//platform dependent macro defines 
#if PSYCH_SYSTEM == PSYCH_WINDOWS

#elif PSYCH_SYSTEM == PSYCH_OS9
	#define FALSE	0
	#define TRUE	1
#elif PSYCH_SYSTEM == PSYCH_OSX
 	#define FALSE	0
	#define TRUE	1
#endif 



//abstract up simple data types. 
#if PSYCH_SYSTEM == PSYCH_WINDOWS
//	typedef BOOL 				boolean;  Windows already defines this.  
	typedef DWORD 				psych_uint32;
	typedef BYTE 				psych_uint8;
	typedef WORD				psych_uint16;
        typedef GLubyte				psych_uint8;
        typedef GLubyte				ubyte;		
#elif PSYCH_SYSTEM == PSYCH_OS9
	typedef Boolean				boolean;
	typedef unsigned long 			psych_uint32;
	typedef Byte				psych_uint8;
	typedef unsigned short 			psych_uint16;
        typedef GLubyte				psych_uint8;
        typedef GLubyte				ubyte;		
#elif PSYCH_SYSTEM == PSYCH_OSX
        typedef Boolean				boolean;
        typedef GLubyte				psych_uint8;
        typedef GLubyte				ubyte;
        typedef UInt32				psych_uint32;
        typedef unsigned long long		psych_uint64;		
#endif
 

#if PSYCH_LANGUAGE == PSYCH_MATLAB
	#undef printf
//	#define printf mexPrintfPtr
	#define printf mexPrintf
	typedef const mxArray CONSTmxArray;
        #define PsychGenericScriptType mxArray
        typedef mxLogical PsychNativeBooleanType; 
#endif

//macros
#if PSYCH_SYSTEM == PSYCH_WINDOWS
	#define EXP __declspec(dllexport)
#else
	#define EXP  //nothing
#endif
	

//end include once
#endif

