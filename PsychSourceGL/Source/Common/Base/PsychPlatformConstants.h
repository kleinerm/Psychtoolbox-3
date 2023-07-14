/*
  PsychToolbox2/Source/Common/PsychPlatformConstants.h		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All
  
  PROJECTS:
  07/16/02	awi		Screen on MacOS9 and MacOS X
   

  HISTORY:
  07/16/02  awi		Pulled out of PsychPlatform.h
  
  DESCRIPTION:
  
  PsychPlatformConstants defines constants which identify all OSes and binding 
  languages
  
  PsychPlatformContstants is at the base of the PsychToolbox header file inclusion
  hierarchy.  Psychtoolbox projects and libraries should include PsychConstants.h 
  or Psych.h, and not include PsychPlatformConstants directly.  
  
  The inclusion order is this:
  
  1> PsychPlatformConstants.h  
    Defines constants which identiy all OSes and binding languages.
  2> PsychPlatform.h
    Defines constants which assert the platform, language, and debugging. 
  3> PsychIncludes.h 
    Includes all standard C, language, and system headers.
  4> PsychConstants
    Abstracts types.
  5a> MiniBox.h
  5b> ...
  6> Psych.h
    Includes everything a PsychToolbox project should need. 
   
  

  
  T0 DO: 
	  

*/
 

//each block is a list of mutually exclusive options.  

//define the platform constant values for PSYCH_SYSTEM in PsychPlatform.h
#define PSYCH_LINUX 		1
#define PSYCH_OS9		2
#define PSYCH_OSX		3
#define PSYCH_IRIX		4
#define PSYCH_WINDOWS		5 

//define the interface language constants for PSYCH_LANGUAGE in PsychPlatform.h
#define PSYCH_MATLAB 		1
#define PSYCH_OCTAVE		2
#define PSYCH_PYTHON		4
#define PSYCH_C			8

//define on and off constants for PSYCH_DEBUG in PsychPlatform.h .  
#define PSYCH_ON		1
#define PSYCH_OFF		0

//define the windowing API.  
#define PSYCH_NO_WINDOW		0 
//Simple Direct Media Layer (SDL)
#define PSYCH_SDL_WINDOW    	1
//Apple's Core Graphics Direct Display and CGL
#define PSYCH_CGL_WINDOW	2

 
// Is this a build for GNU/Octave? If the special compiler flag PTBOCTAVE
// is defined, then it is the octave build and we set the language already
// here, at the lowest layer: Otherwise the PsychPlatform.h file will select
// an appropriate language.
#ifdef PTBOCTAVE
#define PSYCH_LANGUAGE          PSYCH_OCTAVE
#endif

