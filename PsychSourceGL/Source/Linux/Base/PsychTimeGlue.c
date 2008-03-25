/*

  	PsychToolbox3/Source/Linux/Base/PsychTimeGlue.c

	AUTHORS:

  	mario.kleiner at tuebingen.mpg.de		mk 

	PLATFORMS: GNU/Linux Only


  	PROJECTS:

  	HISTORY:

  	2/20/06       mk		Wrote it. Derived from Windows version.  

  	DESCRIPTION:
	
	Functions for querying system time and for waiting for either a
	specified amount of time or until a specified point in time.
	Also returns timer ticks and resolution of timers.

	TO DO:

*/



#include "Psych.h"
#include <time.h>
#include <errno.h>

/*
 *		file local state variables
 */

static double		precisionTimerAdjustmentFactor=1;
static double		estimatedGetSecsValueAtTickCountZero;
static Boolean		isKernelTimebaseFrequencyHzInitialized=FALSE;
static double	        kernelTimebaseFrequencyHz;
static double           sleepwait_threshold = 0.01;
static double		clockinc = 0;

void PsychWaitUntilSeconds(double whenSecs)
{
  struct timespec rqtp;
  double targettime;
  static unsigned int missed_count=0;
  double now=0.0;
  int rc;

  // Get current time:
  PsychGetPrecisionTimerSeconds(&now);

  // If the deadline has already passed, we do nothing and return immediately:
  if (now >= whenSecs) return;

  // Waiting stage 1: If we have more than sleepwait_threshold seconds left
  // until the deadline, we call the OS usleep() function, so the
  // CPU gets released for (difference - sleepwait_threshold) seconds to other processes and threads.
  // -> Good for general system behaviour and for lowered power-consumption (longer battery runtime for
  // Laptops) as the CPU can go idle if nothing else to do...

  // Set an absolute deadline of whenSecs - sleepwait_threshold. We busy-wait the last few microseconds
  // to take scheduling jitter/delays gracefully into account:
  targettime    = whenSecs - sleepwait_threshold;

  // Convert targettime to timespec for the Posix clock functions:
  rqtp.tv_sec   = (unsigned long long) targettime;
  rqtp.tv_nsec = ((targettime - (double) rqtp.tv_sec) * (double) 1e9);  

  // Use clock_nanosleep() to high-res sleep until targettime, repeat if that gets
  // prematurely interrupted for whatever reason...
  while(now < targettime) {
    // MK: Oldstyle - obsolete: usleep((unsigned long)((whenSecs - now - sleepwait_threshold) * 1000000.0f));

    // Starting in 2008, we use high-precision/high-resolution POSIX realtime timers for precise waiting:
    // Call clock_nanosleep, use the realtime wall clock instead of the monotonic clock -- monotonic would
    // by theoretically a bit better as NTP time adjustments couldn't mess with our sleep, but that would
    // cause inconsistencies to other times reported by different useful system services which all measure
    // against wall clock, and in practice, the effect of NTP adjustments is minimal or negligible, as these
    // never create backwards running time or large timewarps, only 1 ppm level adjustments per second, ie,
    // the effect is way below the sleepwait_threshold for any reasonable sleep time -- easily compensated by
    // our hybrid approach...
    // We use TIMER_ABSTIME, so we are totally drift-free and restartable in case our sleep gets interrupted by
    // signals. If clock_nanosleep gets EINTR - Interrupted by a posix signal, we simply loop and restart the
    // sleep. If it returns a different error condition, we abort sleep iteration -- something would be seriously
    // wrong... 
    if ((rc = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &rqtp, NULL)) && (rc != EINTR)) break;

    // Update our 'now' time for reiterating or continuing with busy-sleep...
    PsychGetPrecisionTimerSeconds(&now);
  }

  // Waiting stage 2: We are less than sleepwait_threshold seconds away from deadline.
  // Perform busy-waiting until deadline reached:
  while(now < whenSecs) PsychGetPrecisionTimerSeconds(&now);

  // Check for deadline-miss of more than 0.1 ms:
  if (now - whenSecs > 0.0001) {
    // Deadline missed by over 0.1 ms.
    missed_count++;
    // As long as the threshold is below a msec, immediately increase by 100 microsecs...
    if (sleepwait_threshold < 0.001) sleepwait_threshold+=0.0001;

    // If threshold has reached 1 msec, we require multiple consecutive misses before increasing any further:
    if (missed_count>5) {
      // Too many consecutive misses. Increase our threshold for sleep-waiting
      // by 0.1 ms until it reaches max. 10 ms.
      if (sleepwait_threshold < 0.01) sleepwait_threshold+=0.0001;
      printf("PTB-WARNING: Wait-Deadline missed for %i consecutive times (Last miss %lf ms). New sleepwait_threshold is %lf ms.\n",
	     missed_count, (now - whenSecs)*1000.0f, sleepwait_threshold*1000.0f);
    }
  }
  else {
    // No miss detected. Reset counter...
    missed_count=0;
  }

  // Ready.
  return;
}

void PsychWaitIntervalSeconds(double delaySecs)
{
  double deadline;
  // Get current time:
  PsychGetPrecisionTimerSeconds(&deadline);

  // Compute deadline in absolute system time:
  deadline+=delaySecs;

  // Wait until deadline reached:
  PsychWaitUntilSeconds(deadline);

  return;
}

double	PsychGetKernelTimebaseFrequencyHz(void)
{
  if(!isKernelTimebaseFrequencyHzInitialized){
    isKernelTimebaseFrequencyHzInitialized=TRUE;
    PsychGetPrecisionTimerTicksPerSecond(&kernelTimebaseFrequencyHz);
  }
  return((double)kernelTimebaseFrequencyHz);
}

// CAUTION: This init routine doesn't get called by all modules that use timing,
// only be GetSecs(), so better don't initialize really crucial stuff here!!!
// We init crucial stuff at first invocation of PsychGetPrecisionTimerSeconds(),
// as that routine is called by both, time queries *and* sleeping/waiting functions,
// so its first invocation is at a safe time - and right before real use of the time glue.
void PsychInitTimeGlue(void)
{
  // TODO: Add Mutex init code for the timeglue mutex!
  
  // Set this, although its totally pointless on our implementation...
  PsychEstimateGetSecsValueAtTickCountZero();
}

/* Called at module shutdown/jettison time: */
void PsychExitTimeGlue(void)
{
	// TODO: Add Mutex teardown code for the timeglue mutex!
	return;
}

void PsychGetPrecisionTimerTicks(psych_uint64 *ticks)
{
  double secs;
  // MK: Simply map current systemtime to microseconds...
  PsychGetPrecisionTimerSeconds(&secs);
  *ticks = (psych_uint64) (secs * 1000000.0 + 0.5);
  return;
}

void PsychGetPrecisionTimerTicksPerSecond(double *frequency)
{struct timespec rqtp;
  // MK: Ok, set this to 1 million. Our timesource is gettimeofday(), which
  // resolves time at microsecond resolution, so one can think of it as a
  // virtual timer with a tickrate of 1 Mhz
  *frequency=1000000.0f;
  return;
}

void PsychGetPrecisionTimerTicksMinimumDelta(psych_uint32 *delta)
{
  struct timespec res;

  // We return the real clock tick resolution in microseconds, as 1 tick == 1 microsec
  // in our implementation.
  clock_getres(CLOCK_REALTIME, &res);
  *delta = (psych_uint32) ((((double) res.tv_sec) + ((double) res.tv_nsec / 1e9)) * 1e6);
}

void PsychGetPrecisionTimerSeconds(double *secs)

{
  static boolean firstTime = TRUE;
  struct timespec res;

  // First time invocation?
  if (firstTime) {
	// We query the real clock tick resolution in secs and store in global clockinc.
	// This is useful as a constraint on sleepwait_threshold etc. for our sleep routines...
	clock_getres(CLOCK_REALTIME, &res);
	clockinc = ((double) res.tv_sec) + ((double) res.tv_nsec / 1.e9);

	// sleepwait_threshold should be significantly higher than the granularity of
	// the underlying system clock, say 100x the resolution, but no higher than 10 msecs,
	// and no lower than 100 microseconds. We start with optimistic 250 microseconds...
	sleepwait_threshold = 0.00025;
	if (sleepwait_threshold < 100 * clockinc) sleepwait_threshold = 100 * clockinc;
	if (sleepwait_threshold > 0.010) sleepwait_threshold = 0.010;
	printf("PTB-INFO: Real resolution of system clock is %1.4f microseconds, dynamic sleepwait_threshold starts with %lf msecs...\n", clockinc * 1e6, sleepwait_threshold * 1e3);

	firstTime = FALSE;
  }

  // We use gettimeofday() - It works with microsecond resolution and
  // is implemented via the highest precision time source on each
  // Linux system, e.g., the processors performance counters on
  // Intel Pentium systems. Actually, the resolution of the underlying
  // clocksource is often much better than 1 microsecond, e.g., nanoseconds,
  // but Linux chooses always the highest precision reliable source, so in
  // case TSC's are broken and HPET's are not available and ACPI PM-Timers
  // aren't available, it could be a worse than 1 usec source, although this
  // is extremely unlikely...
  static double oldss = -1;
  double ss;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  ss = ((double) tv.tv_sec) + (((double) tv.tv_usec) / 1000000.0);

  // Some correctness checks against last queried value, if initialized:
  if (oldss > -1) {
	// Old reference available. We check for monotonicity, ie. if time
	// is not going backwards. That's all we can do, as we don't have access
	// to a reference clock. We can't check for clock halts either, because
	// given a fast machine, or concurrent calls from multiple threads and a
	// low granularity clocksource, it could happen that multiple queries report
	// the same time. However, i (MK) checked the source code of current Linux2.6.22
	// kernels and found that Linux itself contains plenty of checks and measures to
	// make sure the clock is working correctly and to compensate for any conceivable
	// failure. We couldn't do a better job than the kernel in any case. This check
	// is just to spot idiots in front of the keyboard that manually override Linux
	// clocksource selection and basically try to shoot themselves into the leg.
	// It may also be useful is somebody is running a very old Linux kernel without
	// sophisticated checking and for testing/debugging PTB and its error-handling itself by
	// fault-injection... 
	// MK: DISABLED FOR THIS RELEASE: Gives false alarms due to some race-condition when
	// function is called from multiple concurrent threads. Proper fix is known, but i
	// want to get a beta out now and not in a week...
	// if (ss < oldss) {
	if (FALSE) {
		// Time warp detected! Time going backwards!!! Nothing we can do, only report
		// it:
		printf("\n\nPTB-CRITICAL-ERROR: Your systems clock is reporting time to run backwards!!!\n");
		printf("PTB-CRITICAL-ERROR: (Delta %lf secs). This is impossible and indicates some\n", ss - oldss);
		printf("PTB-CRITICAL-ERROR: broken clock hardware or Linux setup!! Stop using this machine\n");
		printf("PTB-CRITICAL-ERROR: for psychophysics immmediately and resolve the problem!!!\n\n");
		fflush(NULL);
	}
  }

  // Init reference timestamp for checking in next call:
  oldss = ss;

  // Assign final time value:
  *secs= ss;  
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

*/
void PsychEstimateGetSecsValueAtTickCountZero(void)
{
  double		nowTicks, nowSecs;

  // MK: Todo - Implement GetTickCout().
  // nowTicks=(double) GetTickCount();
  // PsychGetAdjustedPrecisionTimerSeconds(&nowSecs);
  // estimatedGetSecsValueAtTickCountZero=nowSecs - nowTicks * (1/1000.0f); 

  // This is zero by definition of our counters...
  estimatedGetSecsValueAtTickCountZero=0;
}

double PsychGetEstimatedSecsValueAtTickCountZero(void)
{
  return(estimatedGetSecsValueAtTickCountZero);
}
