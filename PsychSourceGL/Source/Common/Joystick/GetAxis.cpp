#include "StdAfx.h"


char useGetAxis[] = "axisState = JOYSTICK(joystickNumber,'GetAxis',axisNumber)";
char synopsisGetAxis[] = "Given a joystick number and an axis number, return the immediate "
  "state of the specified axis on the specified joystick.  The returned value axisState is an "
  "integer (-32768 to 32768) representing the current position of the axis. It might be necessary to " 
  "impose tolerances on the axisState value to ignore jitter."
  "Use 'GetNumJoysticks' and 'GetNumAxes' to determine maximum legal values for joystickNumber and "
  "buttonNumber arguments to 'GetAxis'. The first joystick and the first button are numbered 1. "
  "It is an error to call 'GetAxis' if the specified joystick has no axes.";  
	

void JOYSTICKGetAxis(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int  numSticks, numAxes, fetchedAxis;
	CONSTmxArray *numArg;
	double stickNum;
	SDL_Joystick *pStick;


	if(joystickTable->giveHelp){GiveHelp(useGetAxis,synopsisGetAxis);return;}
	numArg = joystickTable->joystickNumberArgument; 
	if(numArg == NULL || nlhs > 1 || nrhs > 1 || nrhs < 1 || !mxIsDouble(numArg) || (mxGetM(numArg) * mxGetN(numArg) != 1 ||
		!mxIsDouble(prhs[0]) || mxGetM(prhs[0]) * mxGetN(prhs[0]) != 1)){
		GiveUsageExit(useGetAxis);
	}
	numSticks = SDL_NumJoysticks();
	stickNum = mxGetPr(numArg)[0];
	if(stickNum > numSticks)
		PrintfExit("The joystick number %d passed to JOYSTICK 'GetAxis' exceeds the number of joysticks, %d",stickNum,numSticks);
	if(stickNum < 1)
		PrintfExit("The joystick number passed to JOYSTICK 'GetButton' must be greater than 0");
	pStick = GetJoystickObjFromNum((int)stickNum-1);
	if(pStick == NULL)
		PrintfExit("JOYSTICK 'GetAxis' can not open joystick number %d",stickNum);
	numAxes = SDL_JoystickNumAxes(pStick);
	fetchedAxis = (int)mxGetPr(prhs[0])[0];
	if(fetchedAxis > numAxes || fetchedAxis < 0)
		PrintfExit("The axis number %d passed to JOYSTICK 'GetAxis' is oustide the allowable range",fetchedAxis);
	plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);	
	SDL_JoystickUpdate();
	mxGetPr(plhs[0])[0] = SDL_JoystickGetAxis(pStick, fetchedAxis-1);
	
}
