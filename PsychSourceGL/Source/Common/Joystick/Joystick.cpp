// Joystick.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"



#if PSYCH_SYSTEM == PSYCH_WINDOWS

BOOL WINAPI LibMain(
	HINSTANCE hinstDLL,  //handle to this DLL module
	DWORD fdwReason,     //reason for calling function
	LPVOID lpReserved)	 //reserved

{

	//perform actions based on the reason for calling.
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:

			break;
		case DLL_THREAD_ATTACH:

			break;
		case DLL_THREAD_DETACH:

			break;
		case DLL_PROCESS_DETACH:

			break;
	}
			

	return TRUE;  //successful DLL_PROCESS_ATTACH
}
#endif


static MexFunctionPtr SelectFunction(char *command)
{
	ProjectTable *joystickTable=GetProjectTable();

	// See if help is being requested
	if (command[strlen(command)-1] == '?') {
		joystickTable->giveHelp=TRUE;
		command[strlen(command)-1]=0;
	}
	else {
		joystickTable->giveHelp=FALSE;
	}
	
	// Recommended commands
	if(PsychMatch(command,"GetNumJoysticks"))return &JOYSTICKGetNumJoysticks;
	if(PsychMatch(command,"GetNumButtons"))return &JOYSTICKGetNumButtons;
	if(PsychMatch(command,"GetNumAxes"))return &JOYSTICKGetNumAxes;
	if(PsychMatch(command,"GetNumBalls"))return &JOYSTICKGetNumBalls;
	if(PsychMatch(command,"GetNumHats"))return &JOYSTICKGetNumHats;
	if(PsychMatch(command,"GetButton"))return &JOYSTICKGetButton;
	if(PsychMatch(command,"GetAxis"))return &JOYSTICKGetAxis;
	if(PsychMatch(command,"GetBall"))return &JOYSTICKGetBall;
	if(PsychMatch(command,"GetHat"))return &JOYSTICKGetHat;
	if(PsychMatch(command,"GetJoystickName"))return &JOYSTICKGetJoystickName;
	if(PsychMatch(command,"GetJoystickNumbersFromName"))return &JOYSTICKGetJoystickNumbersFromName;
	if(PsychMatch(command,"Unplug"))return &JOYSTICKUnplug;

	// Unknown command.
	return NULL;
}





/*
ROUTINE: mexFunction
PURPOSE:
	Main entry point.  Serves as a dispatch and handles
	first time initialization.
*/

#define MAX_SYNOPSIS 100
#define MAX_CMD_NAME_LENGTH 100

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{
	ProjectTable *joystickTable=GetProjectTable();
	static Boolean firstTime = TRUE;
	static char *synopsis[MAX_SYNOPSIS];
	MexFunctionPtr f;
	char command[MAX_CMD_NAME_LENGTH];

	// Initialization
	if (firstTime) {
		
		if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
			PrintfExit("Could not initialize SDL: %s\n", SDL_GetError());
		}
		mexPrintf("\nDEBUGGING VERSION OF JOYSTICK\n\n");
		
		
		InitializeSynopsis(synopsis,sizeof(synopsis)/sizeof(synopsis[0]));

		// Install cleanup routine
		mexAtExit(&JoystickExitFunction);
	
		firstTime = FALSE;
	}

	// If no args passed, we just print out a synopsis
	if (nrhs == 0) {	
		int i;
		for (i = 0; synopsis[i] != NULL; i++) {
			printf("%s\n",synopsis[i]);
		}
		return;
	}

	// If windowPtr or [] is passed, remove the arg, and save it for later
	if(IsDefaultMat(prhs[0]) || mxIsNumeric(prhs[0])){
		joystickTable->joystickNumberArgument=prhs[0];
		nrhs--;
		prhs++;
	}else {
		joystickTable->joystickNumberArgument=NULL;
	}

	// Get command from first remaining argument
	if (!mxIsChar(prhs[0])) {
		PrintfExit("Usage: Joystick('Command',...);\n");
	}
	mxGetString(prhs[0],command,sizeof(command));
	nrhs--;
	prhs++;
	// Call the appropriate routine
	f=SelectFunction(command);
	if (f == NULL) {
		PrintfExit("Joystick: unknown '%s' command.",command);
	}
	(*f)(nlhs,plhs,nrhs,prhs);

	// Superfluous tidying up.
	joystickTable->giveHelp=0;
}



void InitializeSynopsis(char *synopsis[],int maxStrings)
{
	int i=0;

	synopsis[i++] = "Usage:";
	synopsis[i++] = "\n% Get information on joystick availability.";
	synopsis[i++] = useGetNumJoysticks;
	synopsis[i++] = useGetJoystickName;
	synopsis[i++] = useGetJoystickNumbersFromName;
	synopsis[i++] = "\n% Get information on joystick configuration.";
	synopsis[i++] = useGetNumButtons;
	synopsis[i++] = useGetNumAxes;
	synopsis[i++] = useGetNumBalls;	
	synopsis[i++] = useGetNumHats;
	synopsis[i++] = "\n% Read joystick states.";
	synopsis[i++] = useGetButton;
	synopsis[i++] = useGetAxis;
	synopsis[i++] = useGetBall;
	synopsis[i++] = useGetHat;
	synopsis[i++] = "\n% Unplug a joystick.";
	synopsis[i++] = useUnplug;
	synopsis[i++] = NULL;
	if (i > maxStrings) {
		PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)maxStrings,(long)i);
	}
}	

