/*
  PsychToolbox2/Source/Common/PsychHelp.h		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All
  
  PROJECTS:
  08/19/02	awi		Screen on MacOS9
   

  HISTORY:
  08/19/02  awi		 
  
  DESCRIPTION:
  
  
  

  
  T0 DO: 
  
  The mexPrintf calls in here should be replaced with printf and printf should be 
  defined in the platform glue file to call mexPrintf when building with Matlab.  
	  

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychHelp
#define PSYCH_IS_INCLUDED_PsychHelp


#include "Psych.h"


void PsychSetGiveHelp(void);
void PsychClearGiveHelp(void);
boolean PsychIsGiveHelp(void);
void PsychPushHelp(char *functionUse, char *functionSynopsis, char *functionSeeAlso);
void PsychGiveHelp(void);
void PsychGiveUsage(void);
void PsychGiveUsageExit(void);
char *PsychGetFunctionName(void);


//end include once
#endif



