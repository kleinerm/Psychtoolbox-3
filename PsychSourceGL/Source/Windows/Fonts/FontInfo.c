/*
  PsychToolbox2/Source/OSX/Fonts/Fonts.c		
  
  PROJECTS: ATS only.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	Mac OS X Only
    

  HISTORY:
  10/23/03  	awi		Created. 
 
  
  DESCRIPTION: 
  
  Common functions used by ATS library.  
  
*/ 

#include "FontInfo.h"



PsychError FontsExitFunction(void)
{
    //clean up stuff here
    PsychFreeFontList();
    return(PsychError_none);
}






            


