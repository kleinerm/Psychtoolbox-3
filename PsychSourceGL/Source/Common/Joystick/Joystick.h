/*

	joystick.h

	

	PLATFORMS:	Windows

				Mac OS 9

			

	

	AUTHORS:

	Allen Ingling		awi		Allen.Ingling@nyu.edu



	HISTORY:

	08/12/01		awi		wrote it.

	11/26/01		awi		modified for OS 9.  Moved declarations for minibox

							functions to minibox.h file.     

	



*/



#include "PsychPlatform.h"

#include "SDL.h"

#include "MiniBox.h"





typedef struct ProjectTable{

	CONSTmxArray	*joystickNumberArgument;

	Boolean			giveHelp; 

	Boolean			ignoreCase;

} ProjectTable;







extern void JOYSTICKGetNumJoysticks(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetNumButtons(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetNumAxes(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetNumBalls(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetNumHats(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetButton(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetAxis(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetBall(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetHat(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKUnplug(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetJoystickName(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);

extern void JOYSTICKGetJoystickNumbersFromName(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);



extern void CloseJoystick(void);

void JoystickExitFunction(void);

extern SDL_Joystick *GetJoystickObjFromNum(int joystickNum);

extern ProjectTable *GetProjectTable(void);

void InitializeSynopsis(char *synopsis[],int maxStrings);





extern char useGetNumJoysticks[],useGetJoystickName[], useGetJoystickNumbersFromName[], useGetNumButtons[], 

			useGetNumAxes[], useGetNumBalls[], useGetNumHats[], useGetButton[], useGetAxis[], useGetBall[], 

			useGetHat[], useUnplug[];

	
