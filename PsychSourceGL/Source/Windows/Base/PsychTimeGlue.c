/*

  	PsychToolbox3/Source/windows/Base/PsychTimeGlue.c

	AUTHORS:

  	mario.kleiner at tuebingen.mpg.de		mk 

	PLATFORMS: Micro$oft Windows Only


  	PROJECTS:

  	12/27/05	mk		Screen, GetSecs, WaitSecs, KbCheck, KbWait, PsychPortAudio on M$-Windows

  	HISTORY:

  	12/27/05	mk		Wrote it. Derived from OS-X version.
	11/14/07 mk		Added tons of checking and error-handling code for broken Windoze timers.
						Added special debug helper functions to be used via GetSecs().  

  	DESCRIPTION:
	
	Functions for querying system time and for waiting for either a
	specified amount of time or until a specified point in time.
	Also returns timer ticks and resolution of timers.

	TO DO:

*/



#include "Psych.h"

/*

 *		file local state variables

*/


static double			precisionTimerAdjustmentFactor=1;
static double			estimatedGetSecsValueAtTickCountZero;
static Boolean			isKernelTimebaseFrequencyHzInitialized=FALSE;
static long double	kernelTimebaseFrequencyHz;
static Boolean       counterExists=FALSE;
static Boolean       firstTime=TRUE;
static double        sleepwait_threshold = 0.003;
static Boolean			Timertrouble = FALSE;
static Boolean 		schedulingtrouble = FALSE;
static double			tickInSecsAtLastQuery = -1;
static double			timeInSecsAtLastQuery = -1;

void PsychWaitUntilSeconds(double whenSecs)
{
  static unsigned int missed_count=0;
  double now=0.0;

  // Get current time:
  PsychGetPrecisionTimerSeconds(&now);

  // If the deadline has already passed, we do nothing and return immediately:
  if (now > whenSecs) return;

  // Waiting stage 1: If we have more than sleepwait_threshold seconds left
  // until the deadline, we call the OS Sleep() function, so the
  // CPU gets released for difference - sleepwait_threshold s to other processes and threads.
  // -> Good for general system behaviour and for lowered
  // power-consumption (longer battery runtime for Laptops) as
  // the CPU can go idle if nothing else to do...
  while(whenSecs - now > sleepwait_threshold) {

	 // Sleep until only sleepwait_threshold away from deadline:
    Sleep((int)((whenSecs - now - sleepwait_threshold) * 1000.0f));

	 // Recheck:
    PsychGetPrecisionTimerSeconds(&now);
  }

  // Waiting stage 2: We are less than sleepwait_threshold s away from deadline.
  // Perform busy-waiting until deadline reached:
  while(now < whenSecs) PsychGetPrecisionTimerSeconds(&now);

  // Check for deadline-miss of more than 1 ms:
  if (now - whenSecs > 0.001) {
    // Deadline missed by over 1 ms.
    missed_count++;

    if (missed_count>5) {
      // Too many consecutive misses. Increase our threshold for sleep-waiting
      // by 5 ms until it reaches 20 ms.
      if (sleepwait_threshold < 0.02) sleepwait_threshold+=0.005;
      printf("PTB-WARNING: Wait-Deadline missed for %i consecutive times (Last miss %lf ms). New sleepwait_threshold is %lf ms.\n",
	     missed_count, (now - whenSecs)*1000.0f, sleepwait_threshold*1000.0f);
		// Reset missed count after increase of threshold:
		missed_count = 0;
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

void PsychInitTimeGlue(void)
{
  PsychEstimateGetSecsValueAtTickCountZero();
}

void PsychGetPrecisionTimerTicks(psych_uint64 *ticks)
{
  LARGE_INTEGER	                count;
  if (QueryPerformanceFrequency(&count) && !Timertrouble) {
    QueryPerformanceCounter(&count);
    *ticks = (psych_uint64) count.QuadPart;
  }
  else {
    *ticks = (psych_uint64) timeGetTime();
  }
  return;
}

void PsychGetPrecisionTimerTicksPerSecond(double *frequency)
{
  LARGE_INTEGER	                counterFreq;

  // High precision timer available?
  if (QueryPerformanceFrequency(&counterFreq) && !Timertrouble) {
    // Yes. Returns its operating frequency:
    *frequency=(double) counterFreq.QuadPart;
  }
  else {
    // No. Return the 1 khZ tickfreq of the system tick.
    *frequency=1000.0f;
  }
  return;
}

void PsychGetPrecisionTimerTicksMinimumDelta(psych_uint32 *delta)

{
  // FIXME: Don't know if this is correct!
  *delta=1;
}

void PsychGetPrecisionTimerSeconds(double *secs)

{
  // This code is taken from the old Windows Psychtoolbox:
  // (VideoToolbox/SecondsPC.c)
  double							ss, ticks, diff;
  static LARGE_INTEGER	   counterFreq;
  LARGE_INTEGER			   count;
  static double				oss=0.0f;
  static double				oldticks=0.0f;
  int								tick1, tick2, hangcount;

	// First time init of timeglue: Set up system for high precision timing,
	// and enable workarounds for broken systems:
  if (firstTime) {
		// Switch the system into high resolution timing mode, i.e.,
		// 1 khZ timer interrupts aka 1 msec timer resolution, for both,
		// the Sleep() command and TimeGetTime() queries. This way, our hybrid
		// sleep-waiting algorithm for PsychWaitUntilSeconds() can work with
		// tight busy-wait transition thresholds and doesn't burn too much
		// CPU time. The timeGetTime() function then gets sufficient granularity -
		// 1 msecs - to be a good reference for our correctness/consistency
		// checks on the high precision timer, and it is a sufficient fallback
		// in case of broken timers.
		// The drawback is increased general interrupt load due to the 1 kHZ IRQ's...
    	if ((timeBeginPeriod(1)!=TIMERR_NOERROR) && (schedulingtrouble == FALSE)) {
		  	// High precision mode failed! Output warning on first failed invocation...
		  	schedulingtrouble = TRUE;
        	printf("PTBCRITICAL -ERROR: PsychTimeGlue - Win32 syscall timeBeginPeriod(1) failed!!! Timing will be inaccurate.\n");
        	printf("PTBCRITICAL -ERROR: Time measurement may be highly unreliable - or even false!!!\n");
        	printf("PTBCRITICAL -ERROR: FIX YOUR SYSTEM! In its current state its not useable for conduction of studies!!!\n");
        	printf("PTBCRITICAL -ERROR: Check the FAQ section of the Psychtoolbox Wiki for more information.\n");

		  	// Increase switching threshold to 10 msecs to take low timer resolution into account:
		  	sleepwait_threshold = 0.010;
    	}    

	 	// This command timeEndPeriod(1); should be used when flushing the MEX file, but
		// we don't do it. Once a PsychTimeGlue function was called, we leave Matlab at
		// high timing precision mode and rely on the OS to revert to standard Windoze
		// behaviour, once the Matlab application is quit/terminated.

		// Next step for broken systems: Bind our Matlab interpreter/PTB main thread to the
		// first cpu core in the system. The only known way to make sure we don't get time
		// readings from different TSCs due to our thread jumping between cpu's. TSC's on
		// a multi-core system are not guaranteed to be synchronized, so if TSC is our timebase,
		// this could lead to time inconsistencies - even time going backwards between queries!!!
		// Drawback: We may not make optimal use of a multi-core system.
		if (SetThreadAffinityMask(GetCurrentThread(), 1)==0) {
		  	// Binding failed! Output warning on first failed invocation...
		  	schedulingtrouble = TRUE;
        	printf("PTBCRITICAL -ERROR: PsychTimeGlue - Win32 syscall SetThreadAffinityMask() failed!!! Timing could be inaccurate.\n");
        	printf("PTBCRITICAL -ERROR: Time measurement may be highly unreliable - or even false!!!\n");
        	printf("PTBCRITICAL -ERROR: FIX YOUR SYSTEM! In its current state its not useable for conduction of studies!!!\n");
        	printf("PTBCRITICAL -ERROR: Check the FAQ section of the Psychtoolbox Wiki for more information.\n");
		}
		
		// Spin-Wait until timeGetTime() has switched to 1 msec resolution:
		hangcount = 0;
		while(hangcount < 100) {
			tick1 = (int) timeGetTime();
			while((tick2=(int) timeGetTime()) == tick1);
			if (tick2 - tick1 == 1) break;
			hangcount++;
		}

		if (hangcount >= 100) {
			// Totally foobared system! Output another warning but try to go on. Checks further below in code
			// will trigger and provide counter measures - as far as this is possible with such a screwed system :-(
			printf("PTB-CRITICAL WARNING! Timing code detected problems with the low precision TIMER in your system hardware!\n");
			printf("PTB-CRITICAL WARNING! It doesn't run at the requested rate of 1 tick per millisecond. Interrupt problems?!?\n");
			printf("PTB-CRITICAL WARNING! Your system is somewhat screwed up wrt. timing!\n");
			printf("PTB-CRITICAL WARNING! It is NOT RECOMMENDED to continue using this machine for studies that require high\n");
			printf("PTB-CRITICAL WARNING! timing precision in stimulus onset or response collection. No guarantees can be made\n");
			printf("PTB-CRITICAL WARNING! wrt. to timing or correctness of any timestamps or stimulus onsets!\n");
	      printf("PTB-CRITICAL WARNING! Check the FAQ section of the Psychtoolbox Wiki for more information.\n\n");
		}

		// Ok, now timeGetTime() should have the requested 1 msec increment rate.

		// Is the high-precision timer supported?
    	counterExists = QueryPerformanceFrequency(&counterFreq);
		if (counterExists) {
			// Initialize old counter values to now:
			QueryPerformanceCounter(&count);
			oss = ((double)count.QuadPart)/((double)counterFreq.QuadPart);
			oldticks = (double) timeGetTime() * 0.001;
		}	
  }
  
  if (counterExists) {
	// Query Performance counter:
   QueryPerformanceCounter(&count);
	// Query system time:
	ticks = (double) timeGetTime();

   ss = ((double)count.QuadPart)/((double)counterFreq.QuadPart);
	timeInSecsAtLastQuery = ss;
	ticks = ticks * 0.001;
	tickInSecsAtLastQuery = ticks;

	// Compute difference (disagreement over elapsed time since last call) between high-precision
	// timer and low-precision timer:
	diff = ((ss - oss) - (ticks - oldticks));

	// Check for wrap-around of ticks: The timeGetTime() wraps around every 49.2 days of uptime.
	if (ticks < oldticks) {
		// Wraparound - No reliable test possible! Fake a situation that makes all checks
		// pass during this call. The next call will then work with checks enabled again...
		diff = 0.0;
	}

	// We don't perform the check at first invokation - Thread scheduling etc. needs to settle,
	// as well as the timeBeginPeriod(1) call above...
	if (firstTime) diff = 0.0;

	// Time running backwards?
	if (ss < oss) {
			Timertrouble = TRUE;
			ss = ticks;
			printf("PTB-CRITICAL WARNING! Timing code detected problems with the high precision TIMER in your system hardware!\n");
			printf("PTB-CRITICAL WARNING! Apparently time is reported as RUNNING BACKWARDS.\n");
			printf("PTB-CRITICAL WARNING! One reason could be a multi-core system with unsynchronized TSC's and buggy platform drivers.\n");
			printf("PTB-CRITICAL WARNING! Will switch back to lower precision/resolution timer (only +/-1 millisecond accuracy).\n");
			printf("PTB-CRITICAL WARNING! It is NOT RECOMMENDED to continue using this machine for studies that require high\n");
			printf("PTB-CRITICAL WARNING! timing precision in stimulus onset or response collection. No guarantees can be made\n");
			printf("PTB-CRITICAL WARNING! wrt. to timing or correctness of any timestamps or stimulus onsets!\n");
	      printf("PTB-CRITICAL WARNING! Check the FAQ section of the Psychtoolbox Wiki for more information.\n\n");
	}

	if (!Timertrouble) {
		// No timer problems yet. Perform check: The old and new high res. timer should not
		// disagree in their increment since last call by more than 250 msecs. If they do,
		// this means that the high precision timer leaped forward, which indicates a faulty
		// Southbridge controller in the machines host chipset - Not a good basis for high precision timing.
		// See Microsoft Knowledge base article Nr. 274323 for further explanation and a list of known bad
		// chipsets.
		if (diff > 0.25) {
			// Mismatch between performance counter and tick counter detected!
			// Performance counter is faulty! Report this to user, then continue
			// by use of the older tick counter as a band-aid.
			Timertrouble = TRUE;
			ss = ticks;
			printf("PTB-CRITICAL WARNING! Timing code detected a FAULTY high precision TIMER in your system hardware!(Delta %f secs).\n", diff);
			printf("PTB-CRITICAL WARNING! Seems the timer sometimes randomly jumps forward in time by over 250 msecs!");
			printf("PTB-CRITICAL WARNING! Will switch back to lower precision/resolution timer (only +/-1 millisecond accuracy).\n");
			printf("PTB-CRITICAL WARNING! It is NOT RECOMMENDED to continue using this machine for studies that require high\n");
			printf("PTB-CRITICAL WARNING! timing precision in stimulus onset or response collection. No guarantees can be made\n");
			printf("PTB-CRITICAL WARNING! wrt. to timing or correctness of any timestamps or stimulus onsets!\n");
			printf("PTB-CRITICAL WARNING! For more information see Microsoft knowledge base article Nr. 274323.\n");
			printf("PTB-CRITICAL WARNING! http://support.microsoft.com/default.aspx?scid=KB;EN-US;Q274323&\n\n");
		}
		else {
			// Check for lags: A lag of 1 msec is normal and expected due to the measurement method.
			// Let's check for a 3 msec lag, so we have a bit of headroom to the expected 1 ms lag:
			if (diff < -0.003) {
				// Mismatch between performance counter and tick counter detected!
				// Performance counter is lagging behind realtime! Report this to user, then continue
				// by use of the older tick counter as a band-aid.
				Timertrouble = TRUE;
				ss = ticks;
				printf("PTB-CRITICAL WARNING! Timing code detected a LAGGING high precision TIMER in your system hardware! (Delta %f secs).\n", diff);
				printf("PTB-CRITICAL WARNING! Seems that the timer sometimes stops or slows down! This can happen on systems with\n");
				printf("PTB-CRITICAL WARNING! processor power management (cpu throttling) and defective platform drivers.\n");				
				printf("PTB-CRITICAL WARNING! Will switch back to lower precision/resolution timer (only +/-1 millisecond accuracy).\n");
				printf("PTB-CRITICAL WARNING! Please try if disabling all power management features of your system helps...\n");
				printf("PTB-CRITICAL WARNING! It is NOT RECOMMENDED to continue using this machine for studies that require high\n");
				printf("PTB-CRITICAL WARNING! timing precision in stimulus onset or response collection. No guarantees can be made\n");
				printf("PTB-CRITICAL WARNING! wrt. to timing or correctness of any timestamps or stimulus onsets!\n");
	        	printf("PTB-CRITICAL WARNING! Check the FAQ section of the Psychtoolbox Wiki for more information.\n\n");
			}
			else {
				// Check passed. Update old timestamps, return result.
				oss = ss;
				oldticks = ticks;
			}
		}
	}
	else {
		// Performance counter works unreliably: Fall back to timeGetTime().
		// This only has 1 msec resolution, but at least it works.
		ss = ticks;
	}	
	// End of high precision timestamping.
  } else {
	// Low precision fallback path for ancient machines: 1 khz tick counter:
    ss = (double) timeGetTime();
    ss = ss * 0.001;
	 tickInSecsAtLastQuery = ss;
  }

  // Finally assign time value:  
  *secs= ss;  

	// Clear the firstTime flag - this was the first time, maybe.
   firstTime = FALSE;

	return;
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

	Note that the tick counter rolls over after a couple of months.
	Its theoretically possible to have machine uptime of that long 
	but its extremely unlikely given that this is Microsoft Windows ;)
	so we don't worry about roll over when calculating. 

*/
void PsychEstimateGetSecsValueAtTickCountZero(void)
{
  double		nowTicks, nowSecs;
  
  nowTicks=(double) timeGetTime();
  PsychGetAdjustedPrecisionTimerSeconds(&nowSecs);
  estimatedGetSecsValueAtTickCountZero=nowSecs - nowTicks * (1/1000.0f); 
}

double PsychGetEstimatedSecsValueAtTickCountZero(void)
{
  return(estimatedGetSecsValueAtTickCountZero);
}

/* Returns value of timeGetTime() based timer (in seconds) last time the
	high precision timer was queried. This is a reference value for checking
	correctness of the high precision timer with external code.
	Windows specific, only used by GetSecs() for Windows...
*/
double PsychGetTimeGetTimeValueAtLastTimeQuery(double* precisionRawtime)
{
	*precisionRawtime = timeInSecsAtLastQuery;
	return(tickInSecsAtLastQuery );
}

/* Windows specific: Only used by GetSecs() - Return encoded timer healthy flags: */
unsigned int PsychGetTimeBaseHealthiness(void)
{
	unsigned int v;
	v=(Timertrouble) ? 1 : 0;
	v+=(schedulingtrouble) ? 2 : 0;
	return(v);
}
