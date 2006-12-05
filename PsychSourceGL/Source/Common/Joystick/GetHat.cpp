#include "StdAfx.h"


char useGetHat[] = "hatPosition = JOYSTICK(joystickNumber,'GetHat',hatNumber)";
char synopsisGetHat[] = "Given a joystick number and a hat number, return the immediate " 
  "position of the specified hat on the specified joystick.  The returned value hatPostion is an "
  "integer (1-9) representing the current position of the hat. You can use named constants such "
  "as HatCentered and HatUp to associate the return values with hat postions.   See help for "  
  "'PsychJoystick' for list of these constants. \n"
  "Use 'GetNumJoysticks' and 'GetNumAxes' to determine maximum legal values for joystickNumber and "
  "hatNumber arguments to 'GetAxis'. The first joystick and the first hat are numbered 1. "
  "It is an error to call 'GetHat' if the specified joystick has no hats.";  
	

void JOYSTICKGetHat(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int  numSticks, numHats, fetchedHat;
	CONSTmxArray *numArg;
	double stickNum;
	SDL_Joystick *pStick;
	Uint8 hatVal; 


	if(joystickTable->giveHelp){GiveHelp(useGetHat,synopsisGetHat);return;}
	numArg = joystickTable->joystickNumberArgument; 
	if(numArg == NULL || nlhs > 1 || nrhs > 1 || nrhs < 1 || !mxIsDouble(numArg) || (mxGetM(numArg) * mxGetN(numArg) != 1 ||
		!mxIsDouble(prhs[0]) || (mxGetM(prhs[0]) * mxGetN(prhs[0])) != 1)){
		GiveUsageExit(useGetHat);
	}
	numSticks = SDL_NumJoysticks();
	stickNum = mxGetPr(numArg)[0];
	if(stickNum > numSticks)
		PrintfExit("The joystick number %d passed to JOYSTICK 'GetHat' exceeds the number of joysticks, %d",stickNum,numSticks);
	if(stickNum < 1)
		PrintfExit("The joystick number passed to JOYSTICK 'GetHat' must be greater than 0");
	pStick = GetJoystickObjFromNum((int)stickNum-1);
	if(pStick == NULL)
		PrintfExit("JOYSTICK 'GetHat' can not open joystick number %d",stickNum);
	numHats = SDL_JoystickNumHats(pStick);
	fetchedHat = (int)mxGetPr(prhs[0])[0];
	if(fetchedHat > numHats || fetchedHat < 0)
		PrintfExit("The axis number %d passed to JOYSTICK 'GetHat' is oustide the allowable range",fetchedHat);
	SDL_JoystickUpdate();
	hatVal = SDL_JoystickGetHat(pStick, fetchedHat-1);
	plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);	
	switch(hatVal){
		case SDL_HAT_CENTERED:	mxGetPr(plhs[0])[0] = 0; break; 
		case SDL_HAT_UP:		mxGetPr(plhs[0])[0] = 1; break; 
		case SDL_HAT_RIGHT:		mxGetPr(plhs[0])[0] = 2; break; 
		case SDL_HAT_DOWN:		mxGetPr(plhs[0])[0] = 3; break;
		case SDL_HAT_LEFT:		mxGetPr(plhs[0])[0] = 4; break; 
		case SDL_HAT_RIGHTUP:	mxGetPr(plhs[0])[0] = 5; break; 
		case SDL_HAT_RIGHTDOWN: mxGetPr(plhs[0])[0] = 6; break; 
		case SDL_HAT_LEFTUP:	mxGetPr(plhs[0])[0] = 7; break; 
		case SDL_HAT_LEFTDOWN:	mxGetPr(plhs[0])[0] = 8; break; 
	}
	
}
