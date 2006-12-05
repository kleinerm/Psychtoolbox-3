/*
  PsychToolbox3/Source/Common/SysBeep/SysBeep.h		
  
  PROJECTS: SysBeep only.
  
  PLATFORMS:  Only OS X so far.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  07/06/03  awi		Created.  Borrowed from OS 9 SysBeep.   
  
  TO DO:
  Conditionally compile parts only on OS X.   

*/

#include "SysBeep.h"


 
PsychError SYSBEEPSysBeep(void) 
{
    double ticksBetweenBeeps, numberOfBeeps;
    int i;
    
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(2));

    //Assign defaults then get inputs which will override defaults if passed
    numberOfBeeps=1;
    ticksBetweenBeeps=20;
    PsychCopyInDoubleArg(1, FALSE, &numberOfBeeps);
    PsychCopyInDoubleArg(2, FALSE, &ticksBetweenBeeps);
    
    for(i=0;i<numberOfBeeps;i++){
        SysBeep(30);	// argument is ignored on most Macs; Apple suggests 30.
        if(i+1 < numberOfBeeps)
            WaitTicks(ticksBetweenBeeps);
    }
    
    return(PsychError_none);	
}




