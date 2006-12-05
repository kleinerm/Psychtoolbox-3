/*

  	PsychToolbox3/Source/OSX/Base/PsychTimeGlue.c

  

	AUTHORS:

  	Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS: Mac Only

  

  	PROJECTS:

  	1/20/03	awi		Screen on OS X

   



  	HISTORY:

  	1/20/03	awi		Wrote it.  

        5/27/05 mk              Add while-loops around mach_wait_until, so no problems with interruptions...

                                New routine PsychWaitUntilSeconds() for waiting until a specific time.

  	DESCRIPTION:

  

	

	TO DO:

		 

*/



#include "Psych.h"

#include <mach/mach_time.h>

#include <sys/sysctl.h>





/*

 *		file local state variables

*/

static double		precisionTimerAdjustmentFactor=1;

static double		estimatedGetSecsValueAtTickCountZero;



static Boolean		isKernelTimebaseFrequencyHzInitialized=FALSE;

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

    // Call mach_wait_unit in an endless loop, because it can fail with retcode>0.

    // In that case we just restart...

    while(mach_wait_until(deadlineAbsTics));   	

}







void PsychWaitIntervalSeconds(double delaySecs)

{

    long double						waitPeriodTicks;	

	kern_return_t					waitResult;

	uint64_t						startTimeAbsTics, deadlineAbsTics;



	startTimeAbsTics = mach_absolute_time();

	if(!isKernelTimebaseFrequencyHzInitialized)

		PsychGetKernelTimebaseFrequencyHz();	

	waitPeriodTicks= kernelTimebaseFrequencyHz * delaySecs;

	deadlineAbsTics= startTimeAbsTics + (uint64_t)waitPeriodTicks;

        while(mach_wait_until(deadlineAbsTics));   	

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



void PsychInitTimeGlue(void)

{

	PsychEstimateGetSecsValueAtTickCountZero();

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










