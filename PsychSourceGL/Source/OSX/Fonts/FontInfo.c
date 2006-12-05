/*
	PsychToolbox2/Source/OSX/FontInfo/Fonts.c		

	PROJECTS: 
	
	

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:	
	
		OS X

	HISTORY:
	
		10/23/03  	awi		Created.
		3/7/06		awi		Changed references from "Font" to "FontInfo".  The function had been previously renamed, but not all references updated. 

	DESCRIPTION: 

  
*/ 

#include "FontInfo.h"



PsychError FontsExitFunction(void)
{
    //clean up stuff here
    PsychFreeFontList();
    return(PsychError_none);
}






            


