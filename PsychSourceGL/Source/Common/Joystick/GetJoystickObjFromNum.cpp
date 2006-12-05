

/*
GetJoystickObjFromNum accepts a joystick number and
returns the corresposponding joystick object. 
If the Joystick can not be oppened, we return NULL. 

Within JOYSTICK joysticks should only be oppened by 
calling GetJoystickObjFromNum, not by calling 
SDL_JoystickOpen.  That's because GetJoystickObjFromNum
keeps track of the object, and if you use  the SDL 
function you loose track of the ojbect.     
*/ 

#include "StdAfx.h"


#define MAXIMUM_NUM_JOYSTICKS 100
 
SDL_Joystick *GetJoystickObjFromNum(int joystickNum)
{
	static SDL_Joystick *pJoysticks[MAXIMUM_NUM_JOYSTICKS];
	static Boolean firstTime=1;
	int i;

	
	if(firstTime){
		for(i=0;i<MAXIMUM_NUM_JOYSTICKS;i++)
			pJoysticks[i] = NULL;
		firstTime = 0;
	}
	if(SDL_JoystickOpened(joystickNum)){
		if(pJoysticks[joystickNum] == NULL)
			PrintfExit("GetJoystickObjFromNum could not find the stored joystick object");
	}
	else{
		pJoysticks[joystickNum] = SDL_JoystickOpen(joystickNum);
	}
	return pJoysticks[joystickNum];
}






		


