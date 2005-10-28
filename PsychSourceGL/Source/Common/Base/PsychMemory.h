/*
  Psychtoolbox2/Source/Common/PsychMemory.h		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All 
  
  PROJECTS:
  09/04/02	awi		Screen on MacOS9
   

  HISTORY:
  09/04/02  awi		Wrote it.
  
  DESCRIPTION:


  TO DO: 
  
  
	  

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychMemory
#define PSYCH_IS_INCLUDED_PsychMemory

//allocate memory which is valid between until control returns to
//the calling module
void *PsychCallocTemp(unsigned long n, unsigned long size);
void *PsychMallocTemp(unsigned long n);

//free memory
#if PSYCH_LANGUAGE == PSYCH_MATLAB
	#define PsychFreeTemp 			mxFree
	//void mxFree(void *ptr);
#endif

//allocate memory which is valid while the module is loaded

/*
	For Matlab this should 
	1- allocate memory with mxCalloc or mxMalloc
	2- call mexMakeMemoryPersistent on the pointer
	3- store the pointer in a linked list of pointers 
	   to memory which should be freed with MexAtExit
	   when the module is purged
	   
	do something similar on cleanup for environments where we use
	genuine malloc and calloc and free instead of the matlab 
	versions.
*/


//end include once
#endif
