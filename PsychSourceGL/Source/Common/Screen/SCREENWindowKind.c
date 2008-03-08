/*

	Psychtoolbox3/Source/Common/SCREENWindowKind.c		

  

	AUTHORS:

		Allen.Ingling@nyu.edu				awi 
		mario.kleiner at tuebingen.mpg.de   mk
	PLATFORMS:	

		All.

	HISTORY:

		1/27/03		awi		Created.
		10/13/04	awi
		08/07/07	mk		Add new type proxy window, cleanup.

	DESCRIPTION:

		Accept a vector of window pointers and return a vector of constants identifying the types of each window.
		
	TO DO:
*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "kind=Screen(windowPtr, 'WindowKind');";
static char synopsisString[] = 
        "What kind of windowPtr is this? Returns 0 if it's invalid, -1 an offscreen window or a normal texture, 1 "
        "our onscreen, 2 Matlab's onscreen, 3 a non-redrawable texture, 4 a proxy window. \"windowPtr\" can be an array of window "
        "pointers, eg Screen(Screen('Windows'),'WindowKind'). To count Screen's onscreen "
        "windows, do this: sum(1==Screen(Screen('Windows'),'WindowKind')).\n"
		"Please note that the current PTB doesn't have textures of type '3', i.e. all textures "
		"are also useable as offscreen windows and vice versa. Also, it is not possible to refer "
		"to the Matlab window itself in Psychtoolbox-3 so won't every encounter the value '2' - "
		"it only exists for backwards compatibility with old Psychtoolboxes. ";

static char seeAlsoString[] = "";

#define invalid_window_flag_value			0
#define our_offscreen_window_flag_value		-1
#define our_onscreen_window_flag_value		1
#define matlab_window_flag_value			2
#define our_texture_flag_value				3
#define our_proxy_flag_value				4

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

	numWindows = 0;
    PsychAllocInIntegerListArg(1, FALSE, &numWindows, &windowIndices);
    PsychAllocOutDoubleMatArg(1, FALSE, 1, numWindows, 0, &windowTypes);

    for(i=0;i<numWindows;i++) {
		
		// Is i'th index a valid windowIndex or an out-of-range or empty slot?
		if (IsWindowIndex(windowIndices[i])) {
			// This index denotes a slot with a windowRecord. Check if it is
			// valid and sane (should be unless really bad things happened,
			// in which case we shouldn't be here anyway. Abort with internal
			// error if invalid, otherwise return valid winRec windowRecord:
			FindWindowRecord(windowIndices[i], &winRec);
		}
		else {
			// Out of range index or non-occupied slot in window array:
			winRec = NULL;
		}
		
		// Classify winRec: NULL == invalid window (ie. no window). Note that
		// some of these can't be reached because they don't exist in PTB-3,
		// e.g., the "pure texture" type or the Matlab window.
        if(winRec==NULL) windowTypes[i]=invalid_window_flag_value;
        else if(PsychIsOnscreenWindow(winRec)) windowTypes[i]=our_onscreen_window_flag_value;
        else if(PsychIsOffscreenWindow(winRec)) windowTypes[i]=our_offscreen_window_flag_value;
        else if(PsychIsMatlabOnscreenWindow(winRec)) windowTypes[i]=matlab_window_flag_value;
		else if(PsychIsTexture(winRec)) windowTypes[i]=our_texture_flag_value;			
		else if(winRec->windowType == kPsychProxyWindow) windowTypes[i]=our_proxy_flag_value;
    }
	
    return(PsychError_none);
}
