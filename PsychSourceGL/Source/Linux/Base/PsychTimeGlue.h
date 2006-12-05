/*

  	PsychToolbox3/Source/Linux/Base/PsychTimeGlue.h

	AUTHORS:

  	mario.kleiner@tuebingen.mpg.de		mk

	PLATFORMS: Linux Only

 
  	PROJECTS:

  	HISTORY:

  	2/20/06	mk		Wrote it.  

  	DESCRIPTION:

	TO DO:

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychTimeGlue
#define PSYCH_IS_INCLUDED_PsychTimeGlue

#include "Psych.h"

void PsychWaitUntilSeconds(double whenSecs);
void	PsychWaitIntervalSeconds(double seconds);
double	PsychGetKernelTimebaseFrequencyHz(void);
void PsychGetPrecisionTimerTicks(psych_uint64 *ticks);
void PsychGetPrecisionTimerTicksPerSecond(double *frequency);
void PsychGetPrecisionTimerTicksMinimumDelta(psych_uint32 *delta);
void PsychGetPrecisionTimerSeconds(double *secs);
void PsychGetAdjustedPrecisionTimerSeconds(double *secs);
void PsychGetPrecisionTimerAdjustmentFactor(double *factor);
void PsychSetPrecisionTimerAdjustmentFactor(double *factor);
void PsychInitTimeGlue(void);
void PsychEstimateGetSecsValueAtTickCountZero(void);
double PsychGetEstimatedSecsValueAtTickCountZero(void);

//end include once
#endif

