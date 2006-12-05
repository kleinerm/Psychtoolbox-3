/*

	SCREENWindows.c		

  

	AUTHORS:

  

		Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS:	



		Only OS X for now.

		

	HISTORY:

	

		1/24/03  awi		Created. 

 

	DESCRIPTION:

  

		Return a list of the window pointers for all open windows.

  

	TO DO:

  

*/





#include "Screen.h"



// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c

static char useString[] = "windowPtrs=Screen('Windows');";

static char synopsisString[] = 

        "Return a vector of windowPtrs, including on- and off-screen windows. "

        "n=length(Screen('Windows')) is a handy way to count them. ";

static char seeAlsoString[] = "";

	 



PsychError SCREENWindows(void) 

{

    PsychWindowRecordType	**windowRecordArray;

    int				i,numWindows;

    double			*windowPointers;

	

    //all sub functions should have these two lines

    PsychPushHelp(useString, synopsisString, seeAlsoString);

    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};



    PsychErrorExit(PsychCapNumOutputArgs(1));

    PsychErrorExit(PsychCapNumInputArgs(0));

    

    PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);

    PsychAllocOutDoubleMatArg(1, FALSE, 1, numWindows, 0, &windowPointers);

    for(i=0;i<numWindows;i++)

        windowPointers[i]=windowRecordArray[i]->windowIndex;

    PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);



    return(PsychError_none);	

}






