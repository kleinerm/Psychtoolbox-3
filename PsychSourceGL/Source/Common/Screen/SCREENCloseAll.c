/*

	SCREENCloseAll.c		

  

	AUTHORS:

	

		Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS:	

		

		Only OS X for now.  

    



	HISTORY:

		

		01/23/02	awi		Wrote It.

		10/12/04	awi		Changed "SCREEN" to "Screen" and "Close" to "CloseAll" in useString .

		1/25/04		awi		Merged in a fix provide by mk.  Splits off parts of SCREENCloseAll into ScreenCloseAllWindows()  

							 to be called from ScreenExit(). 



 

    

	T0 DO:

 

*/





#include "Screen.h"



// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c

static char useString[] = "Screen('CloseAll');";

static char synopsisString[] = 

	"Close all open onscreen and offscreen windows and textures";  

static char seeAlsoString[] = "OpenWindow, OpenOffscreenWindow";	 



PsychError SCREENCloseAll(void)

{	

    //all subfunctions should have these two lines.  

    PsychPushHelp(useString, synopsisString, seeAlsoString);

    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    

    PsychErrorExit(PsychCapNumInputArgs(0));   //The maximum number of inputs

    PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs

    

    // Close all windows:

    ScreenCloseAllWindows();

    

    return(PsychError_none);	

}



// Implement closing of all onscreen- and offscreen windows, release of all captured displays,

// releasing of all internal textures and memory buffers, stopping of internal helper threads,

// etc....

// This routine is normally called by SCREENCloseAll, but can be also called by the exit-handler,

// and diverse error-handlers for cleanup.

void ScreenCloseAllWindows()

{

    PsychWindowRecordType	**windowRecordArray;

    int						i, numWindows, numScreens; 



    //close the windows

    PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);

    for(i=0;i<numWindows;i++)

        PsychCloseWindow(windowRecordArray[i]);

    PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);

    

    //release all captured displays

    numScreens=PsychGetNumDisplays();

    for(i=0;i<numScreens;i++){

        if(PsychIsScreenCaptured(i))

            PsychReleaseScreen(i);

    }

    

    return;

}





