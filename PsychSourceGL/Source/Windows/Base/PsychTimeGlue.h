/*

  	PsychToolbox3/Source/Windows/Base/PsychTimeGlue.h

  	AUTHORS:

  	mario.kleiner@tuebingen.mpg.de	mk

	PLATFORMS: Win32 only

  	PROJECTS:

  	1/20/03	awi		Screen on OS X

  	HISTORY:

  	1/20/03	awi		Wrote it for OS/X.  
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
double PsychGetTimeGetTimeValueAtLastTimeQuery(double* precisionRawtime);
unsigned int PsychGetTimeBaseHealthiness(void);
int	PsychInitMutex(psych_mutex* mutex);
int	PsychDestroyMutex(psych_mutex* mutex);
int PsychLockMutex(psych_mutex* mutex);
int PsychUnlockMutex(psych_mutex* mutex);

//end include once
#endif
