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

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychHelp
#define PSYCH_IS_INCLUDED_PsychHelp

#include "Psych.h"

void PsychSetGiveHelp(void);
void PsychClearGiveHelp(void);
psych_bool PsychIsGiveHelp(void);
void PsychPushHelp(char *functionUse, char *functionSynopsis, char *functionSeeAlso);
void PsychGiveHelp(void);
void PsychGiveUsage(void);
void PsychGiveUsageExit(void);
char *PsychGetFunctionName(void);
void PsychOneShotReturnHelp(void);

//end include once
#endif
