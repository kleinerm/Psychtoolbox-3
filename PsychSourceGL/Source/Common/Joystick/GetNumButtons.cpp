#include "StdAfx.h"

char useGetNumButtons[] = "numButtons = JOYSTICK(joystickNumber,'GetNumButtons')";
char synopsisGetNumButtons[] = "Given a joystick number return the number of buttons "
  "on the joystick. You can then use JOYSTICK 'GetButton' to find the state of each button. "; 
	

void JOYSTICKGetNumButtons(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int  numSticks;
	CONSTmxArray *numArg;
	double stickNum;
	SDL_Joystick *pStick;

	prhs;
	if(joystickTable->giveHelp){GiveHelp(useGetNumButtons,synopsisGetNumButtons);return;}
	numArg = joystickTable->joystickNumberArgument; 
	if(numArg == NULL || nlhs > 1 || nrhs > 0 || !mxIsDouble(numArg) || (mxGetM(numArg) * mxGetN(numArg) != 1))
		GiveUsageExit(useGetNumButtons);
	numSticks = SDL_NumJoysticks();
	stickNum = mxGetPr(numArg)[0];
	if(stickNum > numSticks)
		PrintfExit("The joystick number %d passed to JOYSTICK 'GetNumButtons' exceeds the number of joysticks, %d",stickNum,numSticks);
	if(stickNum < 1)
		PrintfExit("The joystick number passed to JOYSTICK 'GetNumButtons' must be greater than 0");
	pStick = GetJoystickObjFromNum((int)stickNum-1);
	if(pStick == NULL)
		PrintfExit("JOYSTICK 'GetNumButtons' can not open joystick number %d",stickNum);
	plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);	
	mxGetPr(plhs[0])[0] = SDL_JoystickNumButtons(pStick);
		
}
