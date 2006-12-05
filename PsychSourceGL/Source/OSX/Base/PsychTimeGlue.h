/*

  	PsychToolbox3/Source/OSX/Base/PsychTimeGlue.h

  

	AUTHORS:

  	Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS: Mac Only

  

  	PROJECTS:

  	1/20/03	awi		Screen on OS X

   



  	HISTORY:

  	1/20/03	awi		Wrote it.  

  

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














