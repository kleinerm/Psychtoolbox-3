/*

	Psychtoolbox3/Source/Common/SCREENWindowKind.c		

  

	AUTHORS:

  

		Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS:	

	

		Only OS X for now.



	HISTORY:



		1/27/03		awi		Created.

		10/13/04	awi		

 

	DESCRIPTION:

  

		Accept a vector of window pointers and return a vector of constants identifying the types of each window.

  

	TO DO:

  

*/





#include "Screen.h"



// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c

static char useString[] = "kind=Screen(windowPtr, 'WindowKind');";

static char synopsisString[] = 

        "What kind of windowPtr is this? Returns 0 if it's invalid, -1 our offscreen, 1 "

        "our onscreen, 2 Matlab's onscreen, 3 a texture. \"windowPtr\" can be an array of window "

        "pointers, eg Screen(Screen('Windows'),'WindowKind'). To count Screen's onscreen "

        "windows, do this: sum(1==Screen(Screen('Windows'),'WindowKind')). ";

static char seeAlsoString[] = "";

	 

#define invalid_window_flag_value			0

#define our_offscreen_window_flag_value		-1

#define our_onscreen_window_flag_value		1

#define matlab_window_flag_value			2

#define our_texture_flag_value				3



PsychError SCREENWindowKind(void) 

{

    int				i,numWindows, *windowIndices;

    double			*windowTypes;

    PsychWindowRecordType	*winRec;  

	

    //all sub functions should have these two lines

    PsychPushHelp(useString, synopsisString, seeAlsoString);

    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};



    PsychErrorExit(PsychCapNumOutputArgs(1));

    PsychErrorExit(PsychCapNumInputArgs(1));

    

    PsychAllocInIntegerListArg(1, 1, &numWindows, &windowIndices);

    PsychAllocOutDoubleMatArg(1, FALSE, 1, numWindows, 0, &windowTypes);

    for(i=0;i<numWindows;i++){

        FindWindowRecord(windowIndices[i], &winRec);

        if(winRec==NULL)

            windowTypes[i]=invalid_window_flag_value;

        else if(PsychIsOnscreenWindow(winRec))

            windowTypes[i]=our_onscreen_window_flag_value;

        else if(PsychIsOffscreenWindow(winRec))

            windowTypes[i]=our_offscreen_window_flag_value;

        else if(PsychIsMatlabOnscreenWindow(winRec))

            windowTypes[i]=matlab_window_flag_value;

		else if(PsychIsTexture(winRec))

			windowTypes[i]=our_texture_flag_value;

    }

    

    return(PsychError_none);	

}














