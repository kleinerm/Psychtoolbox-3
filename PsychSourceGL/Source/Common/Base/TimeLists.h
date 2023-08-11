/*
	TimeLists.h		

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS: 
	
		Only OS X for now.


	HISTORY:
	
		1/18/05		awi		Wrote it. 

	DESCRIPTION:

		For purposes of instrumenting Screen, maintain times samples in an abstract list type.  Internally we use a 
		linked list.  To external functions reading out values, it appears to be an array.
*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_TimeLists
#define PSYCH_IS_INCLUDED_TimeLists


void StoreNowTime(void);
void ClearTimingArray(void);
unsigned int GetNumTimeValues(void);
unsigned int GetTimeArraySizeBytes(void);
void CopyTimeArray(double *destination, unsigned int numElements);

//end include once
#endif




