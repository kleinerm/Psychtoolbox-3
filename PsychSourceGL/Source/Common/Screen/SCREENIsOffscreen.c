/*



	Psychtoolbox3/Source/Common/SCREENIsOffscreen.c		

  

	AUTHORS:

	

		Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS:	

	

		Only OS X for now.



	HISTORY:



		1/27/03  awi		Created. 

 

	DESCRIPTION:

  

		Accept a vector of window pointers and return a vector of flags indicating whether the windows are offscreen or onscreen.

  

	TO DO:

  

*/





#include "Screen.h"



// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c

static char useString[] = "isOffscreen=Screen(windowPtr,'IsOffscreen');";

static char synopsisString[] = 

    "Find out whether window is offscreen.  Responses: 1=yes; 0=no";

static char seeAlsoString[] = "WindowKind";

	 



PsychError SCREENIsOffscreen(void) 

{

    int						i,numWindows, *windowIndices;

    PsychWindowRecordType	*winRec;

    PsychFlagListType 		flagList; 

	

    //all sub functions should have these two lines

    PsychPushHelp(useString, synopsisString, seeAlsoString);

    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};



    PsychErrorExit(PsychCapNumOutputArgs(1));

    PsychErrorExit(PsychCapNumInputArgs(1));

    

    PsychAllocInIntegerListArg(1, TRUE, &numWindows, &windowIndices);

    PsychAllocOutFlagListArg(1, FALSE, numWindows, &flagList); //flag lists are opaque.

    for(i=0;i<numWindows;i++){

        FindWindowRecord(windowIndices[i], &winRec);

        if(winRec==NULL)

            PsychErrorExit(PsychError_invalidWindex);

        else

            PsychLoadFlagListElement(i, PsychIsOffscreenWindow(winRec), flagList);

    }

    

    return(PsychError_none);	

}














