

#include "StdAfx.h"

void JoystickExitFunction(void)
{
	CloseJoystick();
	SDL_Quit();
}



void CloseJoystick(void)
{
	int i,numSticks;
	SDL_Joystick *pStick;

	numSticks = SDL_NumJoysticks();
	for(i=0;i<numSticks;i++){
		if(SDL_JoystickOpened(i)){
			pStick = GetJoystickObjFromNum(i);
			SDL_JoystickClose(pStick);
		}
	}

}

