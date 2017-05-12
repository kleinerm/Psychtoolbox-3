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

/* From the land of wtf: thread_policy_set/get used to be defined in <mach/thread_policy.h>,
 * but since10.8 SDK they aren't anymore (there but commented out). Thanks to this advanced
 * Apple brain-damage we now have to define their prototypes ourselves to avoid compiler
 * warnings:
 */
kern_return_t	thread_policy_set(
                                  thread_t					thread,
                                  thread_policy_flavor_t		flavor,
                                  thread_policy_t				policy_info,
                                  mach_msg_type_number_t		count);

kern_return_t	thread_policy_get(
                                  thread_t					thread,
                                  thread_policy_flavor_t		flavor,
                                  thread_policy_t				policy_info,
                                  mach_msg_type_number_t		*count,
                                  boolean_t					*get_default);



void PsychWaitUntilSeconds(double whenSecs);
void PsychWaitIntervalSeconds(double seconds);
void PsychYieldIntervalSeconds(double seconds);
double PsychGetKernelTimebaseFrequencyHz(void);
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
double PsychGetWallClockSeconds(void);
int PsychInitMutex(psych_mutex* mutex);
int PsychDestroyMutex(psych_mutex* mutex);
int PsychLockMutex(psych_mutex* mutex);
int PsychTryLockMutex(psych_mutex* mutex);
int PsychUnlockMutex(psych_mutex* mutex);
int PsychCreateThread(psych_thread* threadhandle, void* threadparams, void *(*start_routine)(void *), void *arg);
int PsychDeleteThread(psych_thread* threadhandle);
int PsychAbortThread(psych_thread* threadhandle);
void PsychTestCancelThread(psych_thread* threadhandle);
psych_threadid PsychGetThreadId(void);
int PsychIsThreadEqual(psych_thread threadOne, psych_thread threadTwo);
int PsychIsCurrentThreadEqualToId(psych_threadid threadId);
int PsychSetThreadPriority(psych_thread* threadhandle, int basePriority, int tweakPriority);
void PsychSetThreadName(const char *name);
int PsychInitCondition(psych_condition* condition, const pthread_condattr_t* condition_attribute);
int PsychDestroyCondition(psych_condition* condition);
int PsychSignalCondition(psych_condition* condition);
int PsychBroadcastCondition(psych_condition* condition);
int PsychWaitCondition(psych_condition* condition, psych_mutex* mutex);
int PsychTimedWaitCondition(psych_condition* condition, psych_mutex* mutex, double maxwaittimesecs);
#define PsychIsMSVista() 0
int PsychIsCurrentThreadEqualToPsychThread(psych_thread threadhandle);
psych_uint64 PsychAutoLockThreadToCores(psych_uint64* curCpuMask);
int PsychGetOSXMinorVersion(void);
const char* PsychSupportStatus(void);

//end include once

#endif
