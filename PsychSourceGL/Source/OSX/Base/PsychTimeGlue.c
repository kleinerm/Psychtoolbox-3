/*
  	PsychToolbox3/Source/OSX/Base/PsychTimeGlue.c

	AUTHORS:

  	Allen.Ingling@nyu.edu			awi 
  	mario.kleiner@tuebingen.mpg.de	mk

	PLATFORMS: Mac Only

  	PROJECTS:

  	1/20/03	awi		Screen on OS X

  	HISTORY:

  	1/20/03		awi		Wrote it.  

	5/27/05		mk      Add while-loops around mach_wait_until, so no problems with interruptions...
						New routine PsychWaitUntilSeconds() for waiting until a specific time.
						
	1/03/09		mk		Add generic Mutex locking support as service to ptb modules. Add PsychYieldIntervalSeconds().

  	DESCRIPTION:

	TO DO:

*/

#include "Psych.h"
#include <mach/mach_time.h>
#include <sys/sysctl.h>
#include <sched.h>

/*
 *		file local state variables
 */

static double		precisionTimerAdjustmentFactor=1;
static double		estimatedGetSecsValueAtTickCountZero;
static psych_bool		isKernelTimebaseFrequencyHzInitialized=FALSE;
static long double	kernelTimebaseFrequencyHz;

/*
 *		functions
*/

void PsychWaitUntilSeconds(double whenSecs)
{
    kern_return_t					waitResult;
    uint64_t						deadlineAbsTics;

    // Initialize our timebase constant if that hasn't been already done:
    if(!isKernelTimebaseFrequencyHzInitialized) PsychGetKernelTimebaseFrequencyHz();	

    // Compute deadline for wakeup in mach absolute time units:
    deadlineAbsTics= (uint64_t) (kernelTimebaseFrequencyHz * ((long double) whenSecs));

	if (!(deadlineAbsTics > 0 && whenSecs > 0)) return;
	
    // Call mach_wait_unit in an endless loop, because it can fail with retcode>0.
    // In that case we just restart...
    while(mach_wait_until(deadlineAbsTics));   	
}

void PsychWaitIntervalSeconds(double delaySecs)
{
    long double						waitPeriodTicks;	
	kern_return_t					waitResult;
	uint64_t						startTimeAbsTics, deadlineAbsTics;

	if (delaySecs <= 0) return;
	startTimeAbsTics = mach_absolute_time();
	if(!isKernelTimebaseFrequencyHzInitialized) PsychGetKernelTimebaseFrequencyHz();	
	waitPeriodTicks= kernelTimebaseFrequencyHz * delaySecs;
	deadlineAbsTics= startTimeAbsTics + (uint64_t) waitPeriodTicks;
	while(mach_wait_until(deadlineAbsTics));
}

/* PsychYieldIntervalSeconds() - Yield the cpu for given 'delaySecs'
 *
 * PsychYieldIntervalSeconds() differs from PsychWaitIntervalSeconds() in that
 * it is supposed to release the cpu to other threads or processes for *at least*
 * the given amount of time 'delaySecs', instead of *exactly* 'delaySecs'.
 *
 * If one wants to wait an exact amount of time, one uses PsychWaitIntervalSeconds().
 * If one just "has nothing to do" for some minimum amount of time, and wants to
 * play nice to other threads/processes and exact timing is not crucial, then
 * this is the routine of choice. Typical use is within polling loops, where one
 * wants to pause between polling cycles and it doesn't matter if the pause takes
 * a bit longer.
 *
 * A 'delaySecs' of <= zero will just release the cpu for the remainder of
 * the current scheduling timeslice. If you don't know what to do, choose a
 * zero setting.
 *
 */
void PsychYieldIntervalSeconds(double delaySecs)
{
	if (delaySecs <= 0) {
		// Yield cpu for remainder of this timeslice:
		sched_yield();
	}
	else {
		// On OS/X we use standard wait ops - they're good for us:
		PsychWaitIntervalSeconds(delaySecs);
	}
}

double	PsychGetKernelTimebaseFrequencyHz(void)
{
    long double						clockPeriodNSecs;	
    mach_timebase_info_data_t		tbinfo;

	if(!isKernelTimebaseFrequencyHzInitialized){
		// Retrieve the mach absolute time timebase.  The kernel expresses the period in two integers, the ratio of which is the clock period.   
		mach_timebase_info(&tbinfo);

		// Calculate the mach timebase period from values reported from the mach kernel.   
		clockPeriodNSecs = ((long double) tbinfo.numer) / ((long double) tbinfo.denom);

		// Convert the mach timebase period from awkward units into frequency in Hz.
		// Frequency in Hz is a convenient form because it makes converting from a period in seconds into a period in mach timebase units easy:
		//  time_interval_in_mach_units= time_interval_in_seconds * clockFrequencyHz;
		kernelTimebaseFrequencyHz = 1000000000.0 / clockPeriodNSecs;
		isKernelTimebaseFrequencyHzInitialized=TRUE;
	}

	return((double)kernelTimebaseFrequencyHz);
}

/* Called at Module init time: */
void PsychInitTimeGlue(void)
{
	PsychEstimateGetSecsValueAtTickCountZero();
}

/* Called at module shutdown/jettison time: */
void PsychExitTimeGlue(void)
{
	// Nothing to do on OS/X yet:
	return;
}

void PsychGetPrecisionTimerTicks(psych_uint64 *ticks)
{
    *ticks=AudioGetCurrentHostTime();
}

void PsychGetPrecisionTimerTicksPerSecond(double *frequency)
{
    *frequency = (double)AudioGetHostClockFrequency();
}

void PsychGetPrecisionTimerTicksMinimumDelta(psych_uint32 *delta)
{
    *delta=AudioGetHostClockMinimumTimeDelta();
}

void PsychGetPrecisionTimerSeconds(double *secs)
{
	double				timeDouble;
	AbsoluteTime		timeAbsTime;
	Nanoseconds			timeNanoseconds;
	UInt64				timeUInt64;

	//Get the time in an AbsolulteTime structure which expresses time as a ratio.
	timeAbsTime=UpTime();

	//Convert the AbsoluteTime structure to nanoseconds stored in an UnsignedWide.
	//UnsignedWide is an opaque type.  Depending on the compiler it is 
	//implemented either as structure holding holding 32-bit high and low parts
	//or as a native long long.  
	timeNanoseconds=AbsoluteToNanoseconds(timeAbsTime);

	//convert the opaque unsigned wide type into a UInt64.  Variant  forms 
	//of the  UnsignedWide type is  why we need to use the UnsignedWideToUInt64() 
	//macro instead of a cast.  If GCC then UnsignedWideToUInt64 resolves to a type recast. 
	timeUInt64=UnsignedWideToUInt64(timeNanoseconds);

	//cast nanoseconds in unsigned wide type to a double
	timeDouble=(double)timeUInt64;

	//divide down to seconds 
	*secs= timeDouble / 1000000000;  
}

void PsychGetAdjustedPrecisionTimerSeconds(double *secs)
{
	double		rawSecs, factor;
	
	PsychGetPrecisionTimerSeconds(&rawSecs);
	PsychGetPrecisionTimerAdjustmentFactor(&factor);
	*secs=rawSecs * precisionTimerAdjustmentFactor;
}

void PsychGetPrecisionTimerAdjustmentFactor(double *factor)
{
	*factor=precisionTimerAdjustmentFactor;
}

void PsychSetPrecisionTimerAdjustmentFactor(double *factor)
{
	precisionTimerAdjustmentFactor=*factor;
}

/*
	PsychEstimateGetSecsValueAtTickCountZero()
	Note that the tick counter rolls over about every 27 months. Its possible to have machine uptime of that long 
	but it seems unlikely so we don't worry about roll over when calculating 
*/
void PsychEstimateGetSecsValueAtTickCountZero(void)
{
	double		nowTicks, nowSecs;
	
	nowTicks=(double)TickCount();
	PsychGetAdjustedPrecisionTimerSeconds(&nowSecs);
	estimatedGetSecsValueAtTickCountZero=nowSecs - nowTicks * (1/60.15); 
}

double PsychGetEstimatedSecsValueAtTickCountZero(void)
{
	return(estimatedGetSecsValueAtTickCountZero);
}


/* Init a Mutex: */
int	PsychInitMutex(psych_mutex* mutex)
{
	return(pthread_mutex_init(mutex, NULL));
}

/* Deinit and destroy a Mutex: */
int	PsychDestroyMutex(psych_mutex* mutex)
{
	return(pthread_mutex_destroy(mutex));
}

/* Lock a Mutex, blocking until mutex is available if it isn't available: */
int PsychLockMutex(psych_mutex* mutex)
{
	return(pthread_mutex_lock(mutex));
}

/* Unlock a Mutex: */
int PsychUnlockMutex(psych_mutex* mutex)
{
	return(pthread_mutex_unlock(mutex));
}

/* Create a parallel thread of execution, invoke its main routine: */
int PsychCreateThread(psych_thread* threadhandle, void* threadparams, void *(*start_routine)(void *), void *arg)
{
	// threadparams not yet used, this line just to make compiler happy:
	(void*) threadparams;
	
	// Return result code of pthread_create - We're a really thin wrapper around this Posix call:
	return( pthread_create(threadhandle, NULL, start_routine, arg) );
}

/* Join a parallel thread - Wait for its termination, then return its result code: */
int PsychDeleteThread(psych_thread* threadhandle)
{
	// Join on the thread, wait for termination:
	int rc = pthread_join(*threadhandle, NULL);
	// Null out now invalid thread handle of dead thread:
	*threadhandle = NULL;
	// Return return code of joined thread:
	return(rc);
}

/* Send abort request to thread: */
int PsychAbortThread(psych_thread* threadhandle)
{
	return( pthread_cancel(*threadhandle) );
}

/* Return handle of calling thread: */
psych_threadid PsychGetThreadId(void)
{
	return( pthread_self() );
}

/* Check if two given thread handles do refer to the same thread: */
int PsychIsThreadEqual(psych_thread threadOne, psych_thread threadTwo)
{
	return( pthread_equal(threadOne, threadTwo) );
}

/* Check if current (invoking) thread has an id equal to given threadid: */
int PsychIsCurrentThreadEqualToId(psych_threadid threadId)
{
	return( PsychGetThreadId() == threadId );
}
