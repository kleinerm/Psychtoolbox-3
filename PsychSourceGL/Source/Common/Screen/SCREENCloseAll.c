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
void ScreenCloseAllWindows(void)
{
    PsychWindowRecordType	**windowRecordArray;
    int						i, numWindows, numScreens; 

	// Reset the "userspaceGL" flag which tells PTB that userspace GL rendering was active
	// due to Screen('BeginOpenGL') command.
	PsychSetUserspaceGLFlag(FALSE);
	
    // Shutdown Quicktime subsystems if active:
    PsychExitMovies();
    PsychExitVideoCapture();

    // Close the windows: We do it reverse (descending) order so textures get closed
    // before the onscreen windows. In theory this shouldn't matter, but in practice,
    // more stress on the PsychCloseWindow() path. If we have bugs there, chances are
    // higher they get exposed this way, which long-term is a good thing(TM).
    PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
    for(i = numWindows - 1; i >= 0; i--) {
		if (PsychPrefStateGet_Verbosity()>5) { printf("PTB-DEBUG: In ScreenCloseAllWindows(): Destroying window %i\n", i); fflush(NULL); }
		PsychCloseWindow(windowRecordArray[i]);
	}
    PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);

    // Release all captured displays, unhide the cursor on each of them:
    numScreens=PsychGetNumDisplays();
    for(i=0;i<numScreens;i++){
        if(PsychIsScreenCaptured(i)) PsychReleaseScreen(i);
		PsychShowCursor(i);
    }

    return;
}

