/*
	PsychSourceGL/Source/Common/MachAbsoluteTimeClockFrequency/MachAbsoluteTimeClockFrequency.c
	
	PLATFORMS:	
	
		Only OS X for now
			
	AUTHORS:
	
		Mario Kleiner		mk
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
		2/17/05             mk      fixed GetBusFrequencyMex: Needs to return mach_time_base, *not* bus frequency!
		4/5/05				awi		Created new MachTimebase project from Mario's fixed GetBusFrequencyMex.
		4/6/05				awi     Re-wrote DESCRIPTION section and changed variable names for readability.  Expanded comments interspersed with code.
		4/7/05				awi		Relocated the parts which do the work into PsychGetKernelTimebaseFrequencyHz() in PsychTimeGlue.c
		4/8/05				awi		Changed the name to "MachAbsoluteTimeClockFrequency" from "MachTimebase".  The Apple documentation refers 
										to this clock as "mach absolute time" so we do also, even though the name is long.
	
	DESCRIPTION 
		
		Return the frequency of the Mach Kernel "absolute timebase clock".  The frequency depends your  hardware, both the model 
		of CPU and a system hardware clock, perhaps the bus clock.
		
		Mach Kernel functions which assign real-time "Time constraint priority" status to threads parameters in Mach "time base" units.
		The counter which clocks time allocated to your thread counts time in these units.  Use the absolute timebase clock frequency 
		returned by MachAbsoluteTimeClockFrequency to convert seconds into absolute timebase units which you pass to functions which set 
		which set priority:
		
		time_interval_in_mach_units= time_interval_in_seconds * clockFrequencyHz;
		
	NOTES
	
		We add a compile flag to the project MachAbsoluteTimeClockFrequency settings: "-Wno-long-double" turns off a warning caused by using 
		type long double.  The warning is:
			use of `long double' type; its size may change in a future release (Long double usage is reported only once for each file.
			To disable this warning, use -Wno-long-double.)
			
		Turning off the warning is probably ok because we do not depend on long doubles being any particular size or precision; if it does change,
		that's not going to break anything.  
			
		The -Wno-long-double flag is appended to the project setting "Other Warning Flags".

*/


#include "MachAbsoluteTimeClockFrequency.h"

#include <mach/mach_time.h>
#include <sys/sysctl.h>


PsychError MACHABSOLUTETIMECLOCKFREQUENCYMachAbsoluteTimeClockFrequency(void) 
{
    double *clockFrequencyHz;
	
    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
	PsychAllocOutDoubleArg(1, kPsychArgOptional, &clockFrequencyHz);
	*clockFrequencyHz=PsychGetKernelTimebaseFrequencyHz();
	
	return(PsychError_none);
}


// The older version, before we moved the part that does the work into our library of Psycthtoolbox core functions.
/*
PsychError MACHTIMEBASEMachTimebase(void) 
{
    int		 mib[2], tempInt;
    size_t	tempIntSize;	
    mach_timebase_info_data_t tbinfo;
    long double clockPeriodNSecs, clockFrequencyHz;
	

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
    // Query system bus frequency:
    mib[0]=CTL_HW;
    mib[1]=HW_BUS_FREQ;
    tempIntSize=sizeof(tempInt);
    sysctl(mib, 2, &tempInt, &tempIntSize, NULL, 0);

    // Return bus-frequency as (optional) second left-hand-side return argument:
	//PsychCopyOutDoubleArg(2, kPsychArgOptional, (double)tempInt);
	    
    mach_timebase_info(&tbinfo);
    // Calculate the mach timebase period from values reported from the mach kernel.   
    clockPeriodNSecs = ((long double) tbinfo.numer) / ((long double) tbinfo.denom);
    // Convert the mach timebase period from awkward units into frequency in Hz.
	// Frequency in Hz is a convenient form because it makes converting from a period in seconds into a period in mach timebase units easy:
	//  time_interval_in_mach_units= time_interval_in_seconds * clockFrequencyHz;
    clockFrequencyHz = 1000000000.0 / clockPeriodNSecs;
    // Return the mach timbase frequency as the first return argument, the one on the left:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double)clockFrequencyHz);
	
	return(PsychError_none);
}
*/




	
