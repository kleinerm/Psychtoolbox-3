/*
  PsychToolbox2/Source/Common/PsychInstrument.c		
    
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	This file should compile on all platforms.
    

  HISTORY:
  3/18/04  awi		Created. 
 
  
*/


#include "Psych.h"


static double instrumentTime;

void	PsychPushClock(void)
{
	PsychGetPrecisionTimerSeconds(&instrumentTime);

}


double  PsychPopClock(void)
{
	return(instrumentTime);
}





