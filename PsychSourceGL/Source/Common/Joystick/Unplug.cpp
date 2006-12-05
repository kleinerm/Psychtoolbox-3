#include "StdAfx.h"


char useUnplug[] = "JOYSTICK('Unplug')";
char synopsisUnplug[] = "Resets the internal state of JOYSTICK function prior to unplugging joysticks.";  
  
	

void JOYSTICKUnplug(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	CONSTmxArray *numArg;

	plhs; prhs;
	if(joystickTable->giveHelp){GiveHelp(useUnplug,synopsisUnplug);return;}
	numArg = joystickTable->joystickNumberArgument; 
	if(numArg != NULL || nlhs > 0 || nrhs > 0 ){
		GiveUsageExit(useUnplug);
	}
	CloseJoystick();
	
}

