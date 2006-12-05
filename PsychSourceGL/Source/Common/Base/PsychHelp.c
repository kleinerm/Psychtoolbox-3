/*
 	PsychToolbox2/Source/Common/PsychHelp.c		
  
 	AUTHORS:
 
 		Allen.Ingling@nyu.edu		awi 
  
 	PLATFORMS: 
 	
 		OS X only for now.
  
  
  	PROJECTS:
  
  		08/19/02	awi		Screen on OS X
   

  	HISTORY:
  
  		08/19/02  awi	
  		04/22/05  dgp Reduced right margin from 80 to 74.
  		10/11/05	awi Cosmetic.
  
  	DESCRIPTION:
  
  	TO DO: 
  

	  

*/

#include "Psych.h"


//declare static variables for internal use by PsychHelp.cpp
static char *functionUseHELP = NULL; 
static char *functionSynopsisHELP = NULL;
static char *functionSeeAlsoHELP = NULL;
static boolean giveHelpHELP=FALSE;


// functions for flipping a flag to indicate whether function help should be given.    
void PsychSetGiveHelp(void)
{
	giveHelpHELP = TRUE;
}

void PsychClearGiveHelp(void)
{
	giveHelpHELP = FALSE;
}

boolean PsychIsGiveHelp(void)
{
	return(giveHelpHELP);
}


// push the help strings onto a shallow stack 1 element deep
void PsychPushHelp(char *functionUse, char *functionSynopsis, char *functionSeeAlso)
{

	functionUseHELP = functionUse;
	functionSynopsisHELP = functionSynopsis;
        functionSeeAlsoHELP = functionSeeAlso;
}	


void PsychGiveHelp(void)
{  
		printf("\nUsage:\n\n%s\n",functionUseHELP);
		if (functionSynopsisHELP != NULL) 
			printf("\n%s\n", BreakLines(functionSynopsisHELP, 80));
}

void PsychGiveUsage(void)
{  
	printf("Usage:\n\n%s",functionUseHELP);
}


void PsychGiveUsageExit(void)
{  
	PrintfExit("Usage:\n\n%s",functionUseHELP);
}





