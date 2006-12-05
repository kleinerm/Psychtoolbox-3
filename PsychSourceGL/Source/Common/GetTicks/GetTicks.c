/*
  PsychToolbox3/Source/Common/GetTicks/GetTicks.c		
  
  PROJECTS: GetTicks
  
  PLATFORMS:  Only OS X so far.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  7/06/03  awi		Created. 

*/

#include "GetTicks.h"


 
PsychError GETTICKSGetTicks(void) 
{
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));

    PsychCopyOutDoubleArg(1, FALSE, (double) TickCount());
    
    return(PsychError_none);	
}




