#include "StdAfx.h"

char useGetNumHats[] = "numHats = JOYSTICK(joystickNumber,'GetNumHats')";
char synopsisGetNumHats[] = "Given a joystick number return the number of Hats "
  "on the joystick. You can then use JOYSTICK 'GetHat' to find the state of each hat. "; 
	

void JOYSTICKGetNumHats(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int  numSticks;
	CONSTmxArray *numArg;
	double stickNum;
	SDL_Joystick *pStick;

	prhs;
	if(joystickTable->giveHelp){GiveHelp(useGetNumHats,synopsisGetNumHats);return;}
	numArg = joystickTable->joystickNumberArgument; 
	if(numArg == NULL || nlhs > 1 || nrhs > 0 || !mxIsDouble(numArg) || (mxGetM(numArg) * mxGetN(numArg) != 1))
		GiveUsageExit(useGetNumHats);
	numSticks = SDL_NumJoysticks();
	stickNum = mxGetPr(numArg)[0];
	if(stickNum > numSticks)
		PrintfExit("The joystick number %d passed to JOYSTICK 'GetNumHats' exceeds the number of joysticks, %d",stickNum,numSticks);
	if(stickNum < 1)
		PrintfExit("The joystick number passed to JOYSTICK 'GetNumHats' must be greater than 0");
	pStick = GetJoystickObjFromNum((int)stickNum-1);
	if(pStick == NULL)
		PrintfExit("JOYSTICK 'GetNumHats' can not open joystick number %d",stickNum);
	plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);	
	mxGetPr(plhs[0])[0] = SDL_JoystickNumHats(pStick);
		
}

