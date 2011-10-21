/*
 	PsychToolbox3/Source/Common/PsychHelp.c		
  
 	AUTHORS:
 
 		Allen.Ingling@nyu.edu		awi 
		mario.kleiner@tuebingen.mpg.de  mk
 
 	PLATFORMS: 
 	
 		All.

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
static psych_bool giveHelpHELP = FALSE;
static psych_bool oneShotReturnHelp = FALSE;

// functions for flipping a flag to indicate whether function help should be given.    
void PsychSetGiveHelp(void)
{
	giveHelpHELP = TRUE;
}

void PsychClearGiveHelp(void)
{
	giveHelpHELP = FALSE;
}

psych_bool PsychIsGiveHelp(void)
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

void PsychOneShotReturnHelp(void)
{
	oneShotReturnHelp = TRUE;
}

void PsychGiveHelp(void)
{  
    PsychGenericScriptType		*cellVector;

	// Special case: Asked to return help in a cell array of strings?
	if (oneShotReturnHelp) {
		// Yes. Return a 3 element cell array of strings, each containing one
		// of the three help text arguments:
		PsychAllocOutCellVector(1, FALSE, 3,  &cellVector);
		PsychSetCellVectorStringElement(0, functionUseHELP, cellVector);
		PsychSetCellVectorStringElement(1, BreakLines(functionSynopsisHELP, 80), cellVector);
		PsychSetCellVectorStringElement(2, functionSeeAlsoHELP, cellVector);
		oneShotReturnHelp = FALSE;

		return;
	}
	
	// No, standard path: Print to console of runtime system:
	printf("\nUsage:\n\n%s\n",functionUseHELP);
	if (functionSynopsisHELP != NULL) printf("\n%s\n", BreakLines(functionSynopsisHELP, 80));
	if (functionSeeAlsoHELP  != NULL) printf("\nSee also: %s\n", BreakLines(functionSeeAlsoHELP, 80));
}

void PsychGiveUsage(void)
{  
	printf("Usage:\n\n%s",functionUseHELP);
}


void PsychGiveUsageExit(void)
{  
	PrintfExit("Usage:\n\n%s",functionUseHELP);
}
