#include "StdAfx.h"


char useGetJoystickNumbersFromName[] = "joystickNumbers = JOYSTICK('GetJoystickNumbersFromName', joystickName)";
char synopsisGetJoystickNumbersFromName[] = "Given a joystick device name return the numbers of all currently "
  "connected joysticks with that name.   The name is the device name assigned by the manufacturer. "
   "Identical joysticks should report the same device name.";
	

void JOYSTICKGetJoystickNumbersFromName(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{

	ProjectTable *joystickTable=GetProjectTable();
	int i, numSticks, *foundSticks, numFoundSticks=0, nameSize;
	char *joystickName;
	double *resultArray;
	
	
	plhs;
	if(joystickTable->giveHelp){GiveHelp(useGetJoystickNumbersFromName,synopsisGetJoystickNumbersFromName);return;}
	if (joystickTable->joystickNumberArgument != NULL || nlhs > 1 || nrhs > 1 || nrhs < 1 || !mxIsChar(prhs[0]))
		GiveUsageExit(useGetJoystickNumbersFromName);

	numSticks = SDL_NumJoysticks();
	foundSticks = (int *)malloc(numSticks * sizeof(int));
	nameSize = mxGetM(prhs[0]) * mxGetN(prhs[0]) * sizeof(mxChar) + 1;
	joystickName = (char *)malloc(nameSize);
	mxGetString(prhs[0],joystickName, nameSize);
	for(i=0;i<numSticks;i++){
		if(strcmp(joystickName, SDL_JoystickName(i)) == 0){
			foundSticks[numFoundSticks] = i+1;
			++numFoundSticks;
		}
	}
	plhs[0] = mxCreateDoubleMatrix(1,numFoundSticks,mxREAL);
	resultArray = mxGetPr(plhs[0]);
	for(i=0;i<numFoundSticks;i++)
		resultArray[i] = foundSticks[i];
	free(foundSticks);
	free(joystickName);
	

}
