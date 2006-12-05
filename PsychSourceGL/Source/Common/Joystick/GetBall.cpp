/*

08/15/01 awi  wrote it
08/16/01 awi  changed the last word of synopsis string to "trackballs".

*/

#include "StdAfx.h"


char useGetBall[] = "[deltaX, deltaY]  = JOYSTICK(joystickNumber,'GetBall',ballNumber)";
char synopsisGetBall[] = "Given a joystick number and an trackball number, return the change in the trackball " 
  "X and Y positions since the previous call to 'GetBall'.\n"
  "Use 'GetNumJoysticks' and 'GetNumBalls' to determine maximum legal values for joystickNumber and "
  "buttonNumber arguments to 'GetBall'. The first joystick and the first ball are numbered 1. "
  "It is an error to call 'GetBall' if the specified joystick has no trackballs.";  
	

void JOYSTICKGetBall(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int  numSticks, numBalls, fetchedBall, ballX, ballY;
	CONSTmxArray *numArg;
	double stickNum;
	SDL_Joystick *pStick;


	if(joystickTable->giveHelp){GiveHelp(useGetBall,synopsisGetBall);return;}
	numArg = joystickTable->joystickNumberArgument; 
	if(numArg == NULL || nlhs > 2 || nrhs > 1 || nrhs < 1 || !mxIsDouble(numArg) || (mxGetM(numArg) * mxGetN(numArg)) != 1 ||
		!mxIsDouble(prhs[0]) || (mxGetM(prhs[0]) * mxGetN(prhs[0]) != 1)){
		GiveUsageExit(useGetBall);
	}
	numSticks = SDL_NumJoysticks();
	stickNum = mxGetPr(numArg)[0];
	if(stickNum > numSticks)
		PrintfExit("The joystick number %d passed to JOYSTICK 'GetBall' exceeds the number of joysticks, %d",stickNum,numSticks);
	if(stickNum < 1)
		PrintfExit("The joystick number passed to JOYSTICK 'GetBall' must be greater than 0");
	pStick = GetJoystickObjFromNum((int)stickNum-1);
	if(pStick == NULL)
		PrintfExit("JOYSTICK 'GetBall' can not open joystick number %d",stickNum);
	numBalls = SDL_JoystickNumBalls(pStick);
	fetchedBall = (int)mxGetPr(prhs[0])[0];
	if(fetchedBall > numBalls || fetchedBall < 0)
		PrintfExit("The ball number %d passed to JOYSTICK 'GetBall' is oustide the allowable range",fetchedBall);
	SDL_JoystickUpdate();
	SDL_JoystickGetBall(pStick, fetchedBall-1, &ballX, &ballY);

	switch(nlhs){
	case 2: 
		plhs[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		mxGetPr(plhs[1])[0] = (double)ballY;
	case 1:
		plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL);
		mxGetPr(plhs[0])[0] = (double)ballX;
	}
}
