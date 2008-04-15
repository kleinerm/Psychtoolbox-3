/*
	RegisterProject.h		
  
	PROJECTS: 
	
		IOPort only.  
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu				awi
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All.
    
	HISTORY:

		12/20/2004	awi		Wrote it.
		04/10/2008	mk		Started to extend/rewrite it to become a full-fledged generic I/O driver (serial port, parallel port, etc...).
 
	DESCRIPTION:
	
		Originally controlled the Code Mercenaries IO Warrior 40 device.
	
		It will hopefully become a generic I/O driver...

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_RegisterProject
#define PSYCH_IS_INCLUDED_RegisterProject

#include "Psych.h"
#include "IOPort.h"

PsychError PsychModuleInit(void);

//end include once
#endif
