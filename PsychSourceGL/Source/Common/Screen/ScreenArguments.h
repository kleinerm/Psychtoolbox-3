/*
  PsychToolbox2/Source/Common/ScreenArguments.h		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All
  
  PROJECTS:
  11/27/02	awi		Screen on MacOS9
   

  HISTORY:
  11/27/02  awi			Wrote it.  
  
  DESCRIPTION:
  
  - WindowBank contains functions for storing records of open 
  onscreen and offscreen windows.
  
  - We don't use real window pointers when we return a window pointer to the scripting 
  environment.  Instead, we return an index which the PsychToolbox maps to a window
  pointer. Since the mapping is always from the index passed from the scripting environment,
  we can implement this with array lookup. 
  
  The window pointer or screen number is frequently passed as a single number, and by using
  our own indices we make things MUCH easier for ourselves when disambiguating the meaning of a 
  windowPointerOrScreenNumber value, because the value itself is a direct and unambiguouis 
  reference.  
    
  
  T0 DO: 
  This file should look really good to outside functions 
  calling in before any effort is made to improve it internally. 
  
  Maybe abbreviate Screen Number "Scrumber" along with Window Index "Windex".  
    	  

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_ScreenArguments
#define PSYCH_IS_INCLUDED_ScreenArguments



#include "Screen.h"


//constants.  

//constants which specifiy the argument positions used if the constant kPsychUseDefaultArgPosition (defined in ScriptingGlue.h) is passed.  
#define kPsychDefaultNumdexArgPosition 		1   
#define kPsychDefaultColorArgPosition		2
#define kPsychDefaultRectArgPosition		3
#define kPsychDefaultDepthArgPosition		4  



// query, put and get windows and screen specifiers by specifying argument position
boolean			PsychAllocInWindowRecordArg(int position, boolean required, PsychWindowRecordType **winRec);
boolean 		PsychAllocInScreenRecordArg(int position, boolean reqired, PsychScreenRecordType **screenRecord);
boolean 		PsychCopyInWindowIndexArg(int position, boolean required, PsychWindowIndexType *windowIndex);
boolean 		PsychCopyInScreenNumberArg(int position, boolean required, int *screenNumber);
boolean			PsychCopyInScreenNumberOrUnaffiliatedArg(int position, boolean required, int *screenNumber);
boolean 		PsychIsWindowIndexArg(int position);
boolean 		PsychIsScreenNumberArg(int position);
boolean			PsychIsScreenNumberOrUnaffiliatedArg(int position);
boolean			PsychIsUnaffiliatedScreenNumberArg(int position);


// put and get color and rect and depth specifiers directly from arguments
boolean 		PsychCopyInColorArg(int position, boolean required, PsychColorType *color);
boolean 		PsychCopyOutColorArg(int position, boolean required, PsychColorType *color);
boolean 		PsychCopyInRectArg(int position, boolean required, PsychRectType rect);
boolean 		PsychCopyOutRectArg(int position, boolean required, PsychRectType rect);
boolean 		PsychAllocOutRectArg(int position, boolean required, const double **rect);
boolean 		PsychCopyInSingleDepthArg(int position, boolean required, PsychDepthType *depth);
boolean			PsychCopyInDepthValueArg(int position, boolean required, int *depth);
boolean 		PsychCopyOutDepthArg(int position, boolean required, PsychDepthType *depths);


//end include once
#endif

