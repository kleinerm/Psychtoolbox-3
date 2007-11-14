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
	  11/14/07		mk			Add debug diagnosis code for Windoze.

	DESCRIPTION:
   
		Return the time in seconds with high precision. On MS-Windows, it allows to
		trigger some debug operations and returns some diagnostic values that allow
		to diagnose timer problems. Not so on Linux or OS/X: They don't have such problems afaik ;-)

*/

#include "GetSecs.h"

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
	 // are broken by design(TM):
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
		}
	 #endif

    return(PsychError_none);	
}
