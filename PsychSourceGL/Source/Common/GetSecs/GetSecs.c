/*
	PsychSourceGL/Source/Common/GetSecs/GetSecs.c		
  
	PROJECTS: 
  
		GetSecs only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu				awi 
		mario.kleiner@tuebingen.mpg.de	mk
  
	PLATFORMS:	
  
		All.
    
	HISTORY:

		1/20/02		awi		Derived the GetSecs project from Screen .
		8/20/02		awi		Added "version" command borrowed from the Screen project. 
		7/07/04		awi		Cosmetic		
		4/6/05		awi		Updated header comments.
	  11/14/07		mk		Add debug diagnosis code for Windoze.
	  11/20/07		mk		Add gettimeofday() and dummy code for OS/X and Linux.

	DESCRIPTION:
   
		Return the time in seconds with high precision. On MS-Windows, it allows to
		trigger some debug operations and returns some diagnostic values that allow
		to diagnose timer problems. Not so on Linux or OS/X: They don't have such problems afaik ;-)

*/

#include "GetSecs.h"

#if PSYCH_SYSTEM != PSYCH_WINDOWS
#include <sys/time.h>
#endif

PsychError GETSECSGetSecs(void) 
{
    double 	*returnValue;  
	 double  referenceValue, realValue;
	 int		healthy, opmode;

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(5));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    //Allocate a return matrix and load it with the depth values.  
    PsychAllocOutDoubleArg(1, FALSE, &returnValue);
    PsychGetAdjustedPrecisionTimerSeconds(returnValue);

	 // Special code for diagnosing problems with TimeGlue on systems that
	 // are broken by design(TM) aka MS-Windows:
	 #if PSYCH_SYSTEM == PSYCH_WINDOWS
		if (PsychCopyInIntegerArg(1, FALSE, &opmode) && (opmode!=0)) {
			referenceValue = PsychGetTimeGetTimeValueAtLastTimeQuery(&realValue);
			healthy = (int) PsychGetTimeBaseHealthiness();
			PsychCopyOutDoubleArg(2, FALSE, referenceValue);
			PsychCopyOutDoubleArg(3, FALSE, realValue);
			PsychCopyOutDoubleArg(4, FALSE, (double) healthy);

			// A positive opmode value allows to change the thread affinity mask of the PTB Thread.
			// The old affinity mask is returned in that case. This to check for TSC sync across cpu cores:
			if (opmode > 0) PsychCopyOutDoubleArg(5, FALSE, (double) SetThreadAffinityMask(GetCurrentThread(), (DWORD) opmode));
			// An opmode setting of smaller than -1 will try to cancel our timeBeginPeriod(1) requests, as
			// automatically done by the PsychTimeGlue: We try to reset the low-res timer to something
			// like its normal 10 or 15 msecs duty cycle. Could help to spot timers that are actually
			// broken, but do work with the new PTB due to the increased IRQ load and therefore the
			// reduced power management. Post-hoc test if timing was reliable in earlier PTB releases.
			if (opmode < -1) timeEndPeriod(1);
		}
	 #else
		// For MacOS/X and Linux, we return gettimeofday() as reference value and
		// some default "no error" healthy flag. Please note that gettimeofday() is just a
		// drift/NTP corrected version of the system timebase mach_absolute_time(), ie the
		// same value as returned as primary time, just with some offset. Therefore its not
		// possible to check for timebase malfunction, only for clock drift against some
		// external timebase, e.g., UTC as determined by NTP protocol.
		if (PsychCopyInIntegerArg(1, FALSE, &opmode) && (opmode!=0)) {
			struct timeval tv;
			gettimeofday(&tv, NULL);
			referenceValue = ((double) tv.tv_sec) + (((double) tv.tv_usec) / 1000000.0); 
			healthy = 0;
			realValue = *returnValue;
			PsychCopyOutDoubleArg(2, FALSE, referenceValue);
			PsychCopyOutDoubleArg(3, FALSE, realValue);
			PsychCopyOutDoubleArg(4, FALSE, (double) healthy);

			// Copy out a fake thread affinity mask of the PTB Thread if requested:
			if (opmode > 0) PsychCopyOutDoubleArg(5, FALSE, 0);
		}
	 #endif
	 
    return(PsychError_none);	
}
