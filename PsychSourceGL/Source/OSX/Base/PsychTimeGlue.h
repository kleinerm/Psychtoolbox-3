/*

  	PsychToolbox3/Source/OSX/Base/PsychTimeGlue.h

	AUTHORS:

  	Allen.Ingling@nyu.edu				awi 
	mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS: Mac Only

  	PROJECTS:

  	1/20/03	awi		Screen on OS X

  	HISTORY:

  	1/20/03	awi		Wrote it.  
	1/03/09	mk		Add generic Mutex locking support as service to ptb modules.

  	DESCRIPTION:

	TO DO:
*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychTimeGlue
#define PSYCH_IS_INCLUDED_PsychTimeGlue

#include "Psych.h"

void PsychWaitUntilSeconds(double whenSecs);
void PsychWaitIntervalSeconds(double seconds);
void PsychYieldIntervalSeconds(double seconds);
double	PsychGetKernelTimebaseFrequencyHz(void);
void PsychGetPrecisionTimerTicks(psych_uint64 *ticks);
void PsychGetPrecisionTimerTicksPerSecond(double *frequency);
void PsychGetPrecisionTimerTicksMinimumDelta(psych_uint32 *delta);
void PsychGetPrecisionTimerSeconds(double *secs);
void PsychGetAdjustedPrecisionTimerSeconds(double *secs);
void PsychGetPrecisionTimerAdjustmentFactor(double *factor);
void PsychSetPrecisionTimerAdjustmentFactor(double *factor);
void PsychInitTimeGlue(void);
void PsychExitTimeGlue(void);
void PsychEstimateGetSecsValueAtTickCountZero(void);
double PsychGetEstimatedSecsValueAtTickCountZero(void);
int	PsychInitMutex(psych_mutex* mutex);
int	PsychDestroyMutex(psych_mutex* mutex);
int PsychLockMutex(psych_mutex* mutex);
int PsychUnlockMutex(psych_mutex* mutex);
//end include once

#endif
