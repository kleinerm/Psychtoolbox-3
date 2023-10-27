/*
	PsychToolbox2/Source/Common/ProjectTable.cpp
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	PLATFORMS:	All
	
	PROJECTS:
  	07/17/02	awi		Screen on MacOS9

	HISTORY:
	07/17/02		awi		wrote it.

	
	DESCRIPTION: 
	
	TO DO: 
	07/17/02 awi  "numbdex" which is a field of ProjectTable is of type CONSTmxArray,
	               , should be changed for use with Non-Matlab stuff.   
	 

	FIXME: THIS FILE IS COMPLETELY UNUSED -- REMOVE IT!
*/


//begin include once 
#ifndef PSYCH_IS_INCLUDED_ProjectTable
#define PSYCH_IS_INCLUDED_ProjectTable


#include "Psych.h"


/*
	TYPE DEFINITIONS
*/

typedef struct ProjectTable{
  //	CONSTmxArray	*numdex;   
	psych_bool			giveHelp; 
	psych_bool			ignoreCase;
} ProjectTable;



/*
	FUNCTION PROTOTYPES
*/

ProjectTable *GetProjectTable(void);


//end include once
#endif




