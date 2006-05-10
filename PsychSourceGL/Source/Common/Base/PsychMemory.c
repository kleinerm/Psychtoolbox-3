/*
  Psychtoolbox2/Source/Common/PsychMemory.c
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All 
  
  PROJECTS:
  09/04/02	awi		Screen on Mac OSX
   

  HISTORY:
  09/04/02  awi		Wrote it.
  
  DESCRIPTION:


  TO DO: 
  
  		See if we even need this for any reason.  It may be vestigial.  
	  

*/

#include "Psych.h"

#if PSYCH_LANGUAGE == PSYCH_MATLAB

// If running on Matlab, we use Matlab's memory manager...
void *PsychCallocTemp(unsigned long n, unsigned long size)
{
  void *ret;
  
  if(NULL==(ret=mxCalloc((size_t)n, (size_t)size))){
    if(size * n != 0)
      PsychErrorExitMsg(PsychError_outofMemory, NULL);
  }
  return(ret);
}

void *PsychMallocTemp(unsigned long n)
{
  void *ret;
  
  if(NULL==(ret=mxMalloc((size_t)n))){
    if(n!=0)
      PsychErrorExitMsg(PsychError_outofMemory,NULL);
  }
  return(ret);
}

#else

// If not running on Matlab, we use our own allocator...
void *PsychCallocTemp(unsigned long n, unsigned long size)
{
  void *ret;
  
  if(NULL==(ret=calloc((size_t)n, (size_t)size))){
    if(size * n != 0)
      PsychErrorExitMsg(PsychError_outofMemory, NULL);
  }
  return(ret);
}

void *PsychMallocTemp(unsigned long n)
{
  void *ret;
  
  if(NULL==(ret=malloc((size_t)n))){
    if(n!=0)
      PsychErrorExitMsg(PsychError_outofMemory,NULL);
  }
  return(ret);
}

void PsychFreeTemp(void* ptr)
{
  free(ptr);
}

#endif

