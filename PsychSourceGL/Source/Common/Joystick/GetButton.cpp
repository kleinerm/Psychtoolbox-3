#include "StdAfx.h"


char useGetButton[] = "buttonState = JOYSTICK(joystickNumber,'GetButton',buttonNumber)";
char synopsisGetButton[] = "Given a joystick number and a button number, return the immediate " 
  "state of the specified button on the specified joystick.  Use 'GetNumJoysticks' "
  "and 'GetNumButtons' to determine maximum legal values for joystickNumber and buttonNumber arguments. "
  "The first joystick and the first button are number 1. It is an error to call 'GetButton' if the "
  "specified joystick has no buttons.";  
  
	

void JOYSTICKGetButton(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int  numSticks, numButtons, fetchedButton;
	CONSTmxArray *numArg;
	double stickNum;
	SDL_Joystick *pStick;


	if(joystickTable->giveHelp){GiveHelp(useGetButton,synopsisGetButton);return;}
	numArg = joystickTable->joystickNumberArgument; 
	if(numArg == NULL || nlhs > 1 || nrhs > 1 || nrhs < 1 || !mxIsDouble(numArg) || (mxGetM(numArg) * mxGetN(numArg) != 1 ||
		!mxIsDouble(prhs[0]) || (mxGetM(prhs[0]) * mxGetN(prhs[0])) != 1)){
		GiveUsageExit(useGetButton);
	}
	numSticks = SDL_NumJoysticks();
	stickNum = mxGetPr(numArg)[0];
	if(stickNum > numSticks)
		PrintfExit("The joystick number %d passed to JOYSTICK 'GetButton' exceeds the number of joysticks, %d",stickNum,numSticks);
	if(stickNum < 1)
		PrintfExit("The joystick number passed to JOYSTICK 'GetButton' must be greater than 0");
	pStick = GetJoystickObjFromNum((int)stickNum-1);
	if(pStick == NULL)
		PrintfExit("JOYSTICK 'GetButton' can not open joystick number %d",stickNum);
	numButtons = SDL_JoystickNumButtons(pStick);
	fetchedButton = (int)mxGetPr(prhs[0])[0];
	if(fetchedButton > numButtons || fetchedButton < 0)
		PrintfExit("The button number %d passed to JOYSTICK 'GetButton' is oustide the allowable range",fetchedButton);
	plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
	SDL_JoystickUpdate();
	mxGetPr(plhs[0])[0] = SDL_JoystickGetButton(pStick, fetchedButton-1);
	
}
