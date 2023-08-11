/*
  Psychtoolbox3/Source/Common/SCREENTestStructures.c		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	
  This file should build on any platform. 

  HISTORY:
  1/14/03  awi		Created. 
 
  DESCRIPTION:
  
  Test the functions in PsychStructureGlue.h.
  
  TO DO:

  
*/
    

#include "Screen.h"


static char useString[] = "struct=Screen('TestStructures')";
static char synopsisString[] = 
	"return a structure";
static char seeAlsoString[] = "";
	 

PsychError SCREENTestStructures(void) 
{
    const char *farmFieldNames[]={"dogs", "chickens", "horses", "owner", "tractors"};
    const char *tractorFieldNames[]={"Ford", "Deer", "Kubota"};

    PsychGenericScriptType	*farmStructArray, *tractorsStructArray; 
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
    //create a structure and populate it.
    PsychAllocOutStructArray(1, FALSE, 1, 5, farmFieldNames, &farmStructArray);
    PsychSetStructArrayDoubleElement("dogs", 0, 1, farmStructArray);
    PsychSetStructArrayDoubleElement("chickens", 0, 5, farmStructArray);
    PsychSetStructArrayDoubleElement("horses", 0, 2, farmStructArray);
    PsychSetStructArrayStringElement("owner", 0, "Farmer Ingling", farmStructArray);

    PsychAllocOutStructArray(-1, FALSE, 1, 3, tractorFieldNames, &tractorsStructArray);
    PsychSetStructArrayDoubleElement("Ford", 0, 1, tractorsStructArray);
    PsychSetStructArrayDoubleElement("Deer", 0, 1, tractorsStructArray);
    PsychSetStructArrayDoubleElement("Kubota", 0, 0, tractorsStructArray);
    PsychSetStructArrayStructElement("tractors",0, tractorsStructArray, farmStructArray);

    return(PsychError_none);	
}


