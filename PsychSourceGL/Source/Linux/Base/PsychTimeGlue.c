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

/*

 *		file local state variables

*/

static double		precisionTimerAdjustmentFactor=1;
static double		estimatedGetSecsValueAtTickCountZero;
static Boolean		isKernelTimebaseFrequencyHzInitialized=FALSE;
static double	        kernelTimebaseFrequencyHz;
static double           sleepwait_threshold = 0.01;

void PsychWaitUntilSeconds(double whenSecs)
{
  static unsigned int missed_count=0;
  double now=0.0;

  // Get current time:
  PsychGetPrecisionTimerSeconds(&now);

  // If the deadline has already passed, we do nothing and return immediately:
  if (now > whenSecs) return;

  // Waiting stage 1: If we have more than sleepwait_threshold seconds left
  // until the deadline, we call the OS usleep() function, so the
  // CPU gets released for difference - sleepwait_threshold s to other processes and threads.
  // -> Good for general system behaviour and for lowered
  // power-consumption (longer battery runtime for Laptops) as
  // the CPU can go idle if nothing else to do...
  while(whenSecs - now > sleepwait_threshold) {
    usleep((unsigned long)((whenSecs - now - sleepwait_threshold) * 1000000.0f));
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
  // MK: TODO - Implement this...
  *ticks = (psych_uint64) 0;
  return;
}

void PsychGetPrecisionTimerTicksPerSecond(double *frequency)
{
  // MK: TODO Implement this properly...
  *frequency=100.0f;
  return;
}

void PsychGetPrecisionTimerTicksMinimumDelta(psych_uint32 *delta)

{
  // FIXME: Don't know if this is correct!
  *delta=1;
}

void PsychGetPrecisionTimerSeconds(double *secs)

{
  // We use gettimeofday() - It works with microsecond resolution and
  // is implemented via the highest precision time source on each
  // Linux system, e.g., the processors performance counters on
  // Intel Pentium systems.
  double ss;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  ss = ((double) tv.tv_sec) + (((double) tv.tv_usec) / 1000000.0);
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

	Note that the tick counter rolls over after a couple of months.
	Its theoretically possible to have machine uptime of that long 
	but its extremely unlikely given that this is Microsoft Windows ;)
	so we don't worry about roll over when calculating. 

*/
void PsychEstimateGetSecsValueAtTickCountZero(void)
{
  double		nowTicks, nowSecs;

  // MK: Todo - Implement GetTickCout().
  // nowTicks=(double) GetTickCount();
  nowTicks=(double) 0.0;
  PsychGetAdjustedPrecisionTimerSeconds(&nowSecs);
  estimatedGetSecsValueAtTickCountZero=nowSecs - nowTicks * (1/1000.0f); 
}

double PsychGetEstimatedSecsValueAtTickCountZero(void)
{
  return(estimatedGetSecsValueAtTickCountZero);
}

