/*
	PsychSourceGL/Source/Common/GetSecs/GetSecs.c		
  
	PROJECTS: 
  
		GetSecs only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X for now.
    
	HISTORY:

		1/20/02		awi		Derived the GetSecs project from Screen .
		8/20/02		awi		Added "version" command borrowed from the Screen project. 
		7/07/04		awi		Cosmetic		
		4/6/05		awi		Updated header comments.

	DESCRIPTION:
   
		Return the time in seconds with high precision.

*/



#include "GetSecs.h"


PsychError GETSECSGetSecs(void) 
{
    double 	*returnValue;  

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
    //Allocate a return matrix and load it with the depth values.  
    PsychAllocOutDoubleArg(1, FALSE, &returnValue);
    PsychGetAdjustedPrecisionTimerSeconds(returnValue);

    return(PsychError_none);	
}




	
