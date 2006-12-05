
#include "StdAfx.h"

char useGetNumJoysticks[] = "numJoysticks = JOYSTICK('GetNumJoysticks')";
char synopsisGetNumJoysticks[] = "Get the number of joysticks connected to the system. "
  "Returns the number of joystick devices currently connected to your computer "
  "Each joystick is assigned a number in the range of 1 to the value returned by 'GetNumJoysticks'. "
  "Use the assigned number to refer to a joystick when calling JOYSTICK "
  "subfunctions such as 'GetButtons' which read the joystick state. "
  "Assigned numbers change when you unplug or add joysticks, or reboot your computer. " 
  "Therefore,  do not hard-code the numbers into you software.  Instead, you can use "
  "JOYSTICK subfunction 'GetJoystickNumberFromName' to identify a particular model of joystick.";  
	

void JOYSTICKGetNumJoysticks(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int numJoysticks;

	plhs; prhs;
	if(joystickTable->giveHelp){GiveHelp(useGetNumJoysticks,synopsisGetNumJoysticks);return;}
	if (joystickTable->joystickNumberArgument != NULL || nlhs > 1 || nrhs > 0)
		GiveUsageExit(useGetNumJoysticks);
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	numJoysticks = SDL_NumJoysticks();
	mxGetPr(plhs[0])[0]=(double)numJoysticks;

}
