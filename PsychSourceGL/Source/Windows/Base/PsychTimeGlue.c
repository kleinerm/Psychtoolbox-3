/*

  	PsychToolbox3/Source/windows/Base/PsychTimeGlue.c

	AUTHORS:

  	mario.kleiner at tuebingen.mpg.de		mk 

	PLATFORMS: Micro$oft Windows Only


  	PROJECTS:

  	12/27/05	mk		Screen on M$-Windows

  	HISTORY:

  	12/27/05	mk		Wrote it. Derived from OS-X version.  

  	DESCRIPTION:
	
	Functions for querying system time and for waiting for either a
	specified amount of time or until a specified point in time.
	Also returns timer ticks and resolution of timers.

	TO DO:

	Replace the busy-waiting spin-loops in WaitUntilSeconds and WaitIntervalSeconds
	by some proper Win32 call to put process to sleep.

*/



#include "Psych.h"
//#include <windows.h>
//#include <windowsx.h>
//#include <windows.h>
//#include <windowsx.h>
//#include <ddraw.h>
//#define DIRECTINPUT_VERSION 0x0500
//#include <dinput.h>
//#include <winbase.h>
//#include <dsetup.h>


/*

 *		file local state variables

*/

static double		precisionTimerAdjustmentFactor=1;
static double		estimatedGetSecsValueAtTickCountZero;
static Boolean		isKernelTimebaseFrequencyHzInitialized=FALSE;
static long double	kernelTimebaseFrequencyHz;
static Boolean          counterExists=FALSE;
static Boolean          firstTime=TRUE;

void PsychWaitUntilSeconds(double whenSecs)
{
  double now=0.0;

  // Waiting stage 1: If we have more than 2.5 milliseconds left
  // until the deadline, we call the OS Sleep(2) function, so the
  // CPU gets released for 2 ms to other processes and threads.
  // -> Good for general system behaviour and for lowered
  // power-consumption (longer battery runtime for Laptops) as
  // the CPU can go idle if nothing else to do...
  PsychGetPrecisionTimerSeconds(&now);
  while(whenSecs - now > 0.0025) {
    Sleep(2);
    PsychGetPrecisionTimerSeconds(&now);
  }

  // Waiting stage 2: We are less than 2.5 ms away from deadline.
  // Perform busy-waiting until deadline reached:
  while(now < whenSecs) PsychGetPrecisionTimerSeconds(&now);

  // Check for deadline-miss of more than 500 microseconds:
  if (now - whenSecs > 0.0005) printf("PTB-WARNING: Wait-Deadline missed by %lf ms.\n", (now - whenSecs)*1000.0f);

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
  if (QueryPerformanceFrequency(&count)) {
    QueryPerformanceCounter(&count);
    *ticks = (psych_uint64) count.QuadPart;
  }
  else {
    *ticks = (psych_uint64) GetTickCount();
  }
  return;
}

void PsychGetPrecisionTimerTicksPerSecond(double *frequency)
{
  LARGE_INTEGER	                counterFreq;

  // High precision timer available?
  if (QueryPerformanceFrequency(&counterFreq)) {
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
  double				ss;
  static LARGE_INTEGER	                counterFreq;
  LARGE_INTEGER			        count;

  if (firstTime) {
    counterExists = QueryPerformanceFrequency(&counterFreq);
    firstTime = FALSE;
  }
  
  if (counterExists) {
    QueryPerformanceCounter(&count);
    ss = ((double)count.QuadPart)/((double)counterFreq.QuadPart);
  } else {
    ss = (double) GetTickCount();
    ss = ss/1000;
  }
  
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
  
  nowTicks=(double) GetTickCount();
  PsychGetAdjustedPrecisionTimerSeconds(&nowSecs);
  estimatedGetSecsValueAtTickCountZero=nowSecs - nowTicks * (1/1000.0f); 
}

double PsychGetEstimatedSecsValueAtTickCountZero(void)
{
  return(estimatedGetSecsValueAtTickCountZero);
}
