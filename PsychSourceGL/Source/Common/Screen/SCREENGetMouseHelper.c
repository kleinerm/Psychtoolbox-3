/*
	SCREENGetMouseHelper.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:
	
		Only OS X for now.
    

	HISTORY:

		10/11/04	awi		Created.  
  
 
	DESCRIPTION:
  
		Returns the position of the mouse pointer.  Experimental.  
		
	NOTES
	
		We need these ingredients:
		
			1. Read mouse buttons
			
				Options are Button(), GetCurrentEventButtonState() or GetCurrentButtonState().  Or use preferences to set which.
			
			2. Detect number of mouse buttons
			
				Could use PsychHID.
			
			3. Read screen position of mouse.
			
				GetMouse, even though it's deprectated.
			
			4. Read screen position of mouse in CG fullscreen window when resolution changes.
			
				TestGetMouse.  We may have to apply a hack if GetMouse does not pick up on the resolution change. 
				
			5. Get the origin for each onscreen window in global coordinates.
			
					Use:
					CGRect CGDisplayBounds (CGDirectDisplayID display);

					Add:
					oldRect=Screen(windowPtrOrScreenNumber,'GlobalRect',[rect]);

*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[x, y, buttonValueArray]= Screen('GetMouseHelper', numButtons);";
//                          1  2  3                                           1  
static char synopsisString[] = 
	"This is a helper function called by GetMouse.  Do not call Screen(\'GetMouseHelper\'), use "
	"GetMouse instead.\n"
	"\"numButtons\" is the number of mouse buttons to return in buttonValueArray. 1 <= numButtons <= 32.";
static char seeAlsoString[] = "";
	 

PsychError SCREENGetMouseHelper(void) 
{
  // FIXME: Unimplemented!!!!
#if PSYCH_SYSTEM == PSYCH_OSX
	Point		mouseXY;
	UInt32		buttonState;
	double		numButtons, *buttonArray;
	int		i;
	boolean		doButtonArray;
	
	
	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//cap the numbers of inputs and outputs
	PsychErrorExit(PsychCapNumInputArgs(1));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(3));  //The maximum number of outputs
	
	//Buttons.  
	// The only way I know to detect the  number number of mouse buttons is directly via HID.  The device reports
	//that information but OS X seems to ignore it above the level of the HID driver, that is, no OS X API above the HID driver
	//exposes it.  So GetMouse.m function calls PsychHID detect the number of buttons and then passes that value to GetMouseHelper 
	//which returns that number of button values in a vector.      
	PsychCopyInDoubleArg(1, kPsychArgRequired, &numButtons);
	if(numButtons > 32)
		PsychErrorExitMsg(PsychErorr_argumentValueOutOfRange, "numButtons must not exceed 32");
	if(numButtons < 1) 
		PsychErrorExitMsg(PsychErorr_argumentValueOutOfRange, "numButtons must exceed 1");
	doButtonArray=PsychAllocOutDoubleMatArg(3, kPsychArgOptional, (int)1, (int)numButtons, (int)1, &buttonArray);
	if(doButtonArray){
		buttonState=GetCurrentButtonState();
		for(i=0;i<numButtons;i++)
			buttonArray[i]=(double)(buttonState & (1<<i));
	}
			
	//cursor position
	GetMouse(&mouseXY);
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double)mouseXY.h);
	PsychCopyOutDoubleArg(2, kPsychArgOptional, (double)mouseXY.v);
#else
	double numButtons;
	double* buttonArray;
	PsychCopyInDoubleArg(1, kPsychArgRequired, &numButtons);
	PsychAllocOutDoubleMatArg(3, kPsychArgOptional, (int)1, (int)numButtons, (int)1, &buttonArray);
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double)0);
	PsychCopyOutDoubleArg(2, kPsychArgOptional, (double)0);
#endif
	
	return(PsychError_none);
	
}


	
	





