/*
  	Psychtoolbox3/Source/Common/PreferenceGlue/PsychPreferences.h
  
	AUTHORS:
  	Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS: MATLAB only
  
  	PROJECTS:
  	8/04/03	awi		PsychPreferences on OS X
   

  	HISTORY:
  	10/08/02  awi		Wrote it.  
  
  	DESCRIPTION:
  	
	TO DO:
	 
*/


//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychPreferences
#define PSYCH_IS_INCLUDED_PsychPreferences

#include "Psych.h"


Boolean IsMATLABMatrixTrue(mxArray *mptr);
Boolean IsPsychPreferencesValid(void);
Boolean PreparePsychPreferences(char *omitMexFunction);
Boolean GetPreferencesFlag(char *flagName);
void SetPreferencesDoubleValue(char *flagName, double value );
void PsychSetPreferencesNativeDoublesArray(char *flagName, mxArray *setMat);

#endif

