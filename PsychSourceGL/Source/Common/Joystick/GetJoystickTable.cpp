/*
	GetJoystickTable.cpp
	
	PLATFORMS:	Windows
				Mac OS 9
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	08/12/01		awi		wrote it.
	11/26/01		awi		Renamed JoystickTable type to ProjectTable  
	

*/

#include "StdAfx.h"

ProjectTable *GetProjectTable(void)
{
	static ProjectTable projectTable;
	static firstTime=1;

	if(firstTime){
		firstTime = 0;
		projectTable.joystickNumberArgument = NULL;
		projectTable.giveHelp = FALSE;
		projectTable.ignoreCase = FALSE;
		//add inits for other fields here
	}
	return &projectTable;
}


	 

