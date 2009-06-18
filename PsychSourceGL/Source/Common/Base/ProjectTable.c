

/*
	PsychToolbox2/Source/Common/ProjectTable.cpp
	
	PLATFORMS:	Windows
				Mac OS 9
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	08/12/01		awi		wrote it.
	11/26/01		awi		Renamed JoystickTable type to ProjectTable
	  
	
	TO DO:  
	Dynamically allocable fields would be really nice.  
*/


// #include "Psych.h"
#define PTBINSCRIPTINGGLUE 1
    #include "Psych.h"
#undef PTBINSCRIPTINGGLUE


ProjectTable *GetProjectTable(void)
{
	static ProjectTable projectTable;
	static int firstTime=1;

	if(firstTime){
		firstTime = 0;
		projectTable.ignoreCase = FALSE;
		//add inits for other fields here
	}
	return(&projectTable);
}


