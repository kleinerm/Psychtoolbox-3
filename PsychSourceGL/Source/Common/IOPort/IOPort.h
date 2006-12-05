/*
	IOPort.h		
  
	PROJECTS: 
	
		IOPort only.  
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
	
		Only OS X for now
    
	HISTORY:

		12/20/2004	awi		Wrote it.
 
	DESCRIPTION:
	
		Controls the Code Mercenaries IO Warrior 40 device.  
	
  
  

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_IOPort
#define PSYCH_IS_INCLUDED_IOPort

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"



//function prototypes
PsychError MODULEVersion(void); 
PsychError IOPORTSetPort(void);
PsychError IOPORTInit(void);

//end include once
#endif

	
