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

void *PsychCallocTemp(unsigned long n, unsigned long size)
{
    void *ret;
	
//    if(NULL==(ret=mxCallocPtr((size_t)n, (size_t)size))){
    if(NULL==(ret=mxCalloc((size_t)n, (size_t)size))){
        if(size * n != 0)
            PsychErrorExitMsg(PsychError_outofMemory, NULL);
    }
    return(ret);
}

void *PsychMallocTemp(unsigned long n)
{
    void *ret;
	
//    if(NULL==(ret=mxMallocPtr((size_t)n))){
	  if(NULL==(ret=mxMalloc((size_t)n))){
        if(n!=0)
            PsychErrorExitMsg(PsychError_outofMemory,NULL);
    }
    return(ret);
}



#endif