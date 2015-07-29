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
psych_bool			PsychAllocInWindowRecordArg(int position, psych_bool required, PsychWindowRecordType **winRec);
psych_bool 		PsychAllocInScreenRecordArg(int position, psych_bool reqired, PsychScreenRecordType **screenRecord);
psych_bool 		PsychCopyInWindowIndexArg(int position, psych_bool required, PsychWindowIndexType *windowIndex);
psych_bool 		PsychCopyInScreenNumberArg(int position, psych_bool required, int *screenNumber);
psych_bool			PsychCopyInScreenNumberOrUnaffiliatedArg(int position, psych_bool required, int *screenNumber);
psych_bool 		PsychIsWindowIndexArg(int position);
psych_bool 		PsychIsScreenNumberArg(int position);
psych_bool			PsychIsScreenNumberOrUnaffiliatedArg(int position);
psych_bool			PsychIsUnaffiliatedScreenNumberArg(int position);


// put and get color and rect and depth specifiers directly from arguments
psych_bool 		PsychCopyInColorArg(int position, psych_bool required, PsychColorType *color);
psych_bool 		PsychCopyOutColorArg(int position, psych_bool required, PsychColorType *color, PsychWindowRecordType *windowRecord);
psych_bool 		PsychCopyInRectArg(int position, psych_bool required, PsychRectType rect);
psych_bool 		PsychCopyOutRectArg(int position, psych_bool required, PsychRectType rect);
psych_bool 		PsychAllocOutRectArg(int position, psych_bool required, const double **rect);
psych_bool 		PsychCopyInSingleDepthArg(int position, psych_bool required, PsychDepthType *depth);
psych_bool			PsychCopyInDepthValueArg(int position, psych_bool required, int *depth);
psych_bool 		PsychCopyOutDepthArg(int position, psych_bool required, PsychDepthType *depths);


//end include once
#endif

