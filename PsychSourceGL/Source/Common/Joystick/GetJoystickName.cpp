#include "StdAfx.h"

char useGetJoystickName[] = "joystickName = JOYSTICK(joystickNumber, 'GetJoystickName')";
char synopsisGetJoystickName[] = "Given a joystick's number return its device name "
  "joystickNumber may be any number beginning with 1 up to the number of connected joysticks. "
  "Use JOYSTICK 'GetNumJoysticks' to find the number of connected joysticks";
	

void JOYSTICKGetJoystickName(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int numSticks;
	const char *joystickName;
	double stickNum;
	
	plhs;
	prhs;
	if(joystickTable->giveHelp){GiveHelp(useGetJoystickName,synopsisGetJoystickName);return;}
	if (joystickTable->joystickNumberArgument == NULL || nlhs > 1 || nrhs > 0 )
		GiveUsageExit(useGetJoystickName);
	numSticks = SDL_NumJoysticks();
	stickNum = mxGetPr(joystickTable->joystickNumberArgument)[0];
	if(stickNum > numSticks)
		PrintfExit("The joystick number %d passed to JOYSTICK 'GetJoystickName' exceeds the number of joysticks, %d",stickNum,numSticks);
	if(stickNum < 1)
		PrintfExit("The joystick number passed to JOYSTICK 'GetJoystickName' must be greater than 0");
	joystickName = SDL_JoystickName((int)(stickNum-1));
	plhs[0] = mxCreateString(joystickName);



}
