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
static char useString[] = "[x, y, buttonValueArray]= Screen('GetMouseHelper', numButtons [, screenNumber]);";
//                          1  2  3                                           1          2
static char synopsisString[] = 
	"This is a helper function called by GetMouse.  Do not call Screen(\'GetMouseHelper\'), use "
	"GetMouse instead.\n"
	"\"numButtons\" is the number of mouse buttons to return in buttonValueArray. 1 <= numButtons <= 32. Ignored on Linux and Windows."
   "\"screenNumber\" is the number of the PTB screen whose mouse should be queried on setups with multiple connected mice. "
   "This value is optional (defaults to zero) and only honored on GNU/Linux. It's meaningless on OS-X or Windows.";
static char seeAlsoString[] = "";
	 

PsychError SCREENGetMouseHelper(void) 
{
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
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
	static unsigned char disabledKeys[256];
	static unsigned char firsttime = 1;
	int keysdown, i;
	unsigned char keyState[256];
	double* buttonArray;
	double numButtons, timestamp;
	PsychNativeBooleanType* buttonStates;
	POINT		point;
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	// Retrieve optional number of mouse buttons:
	numButtons = 0;
	PsychCopyInDoubleArg(1, FALSE, &numButtons);

	// Are we operating in 'GetMouseHelper' mode? numButtons>=0 indicates this.
	if (numButtons>=0) {
		// GetMouse-Mode: Return mouse button states and mouse cursor position:

		PsychAllocOutDoubleMatArg(3, kPsychArgOptional, (int)1, (int)3, (int)1, &buttonArray);
		// Query and return mouse button state:
		PsychGetMouseButtonState(buttonArray);
		// Query and return cursor position in global coordinates:
		GetCursorPos(&point);
		PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) point.x);
		PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) point.y);
	}
	else {
	  // 'KeyboardHelper' mode: We implement either KbCheck() or KbWait() via X11.
	  // This is a hack to provide keyboard queries until a PsychHID() implementation
	  // for Microsoft Windows is available...

	  if (firsttime) {
			// First time init:
			firsttime = 0;
			memset(keyState, 0, sizeof(keyState));
			memset(disabledKeys, 0, sizeof(disabledKeys));
			// These keycodes are always disabled: 0, 255:
			disabledKeys[0]=1;
			disabledKeys[255]=1;
			// Mouse buttone (left, right, middle) are also disabled by default:
			disabledKeys[1]=1;
			disabledKeys[2]=1;
			disabledKeys[4]=1;
	  }

	  if (numButtons==-1 || numButtons==-2) {
	    // KbCheck()/KbWait() mode
	    do {
	      // Reset overall key state to "none pressed":
	      keysdown=0;

	      // Request current time of query:
	      PsychGetAdjustedPrecisionTimerSeconds(&timestamp);

			// Query state of all keys:
			for(i=1;i<255;i++){
				keyState[i] = (GetAsyncKeyState(i) & -32768) ? 1 : 0;
			}

	      // Disable all keys that are registered in disabledKeys. Check if
			// any non-disabled key is down.
	      for (i=0; i<256; i++) {
				if (disabledKeys[i]>0) keyState[i] = 0;
				keysdown+=(unsigned int) keyState[i];
	      }

	      // We repeat until any key pressed if in KbWait() mode, otherwise we
	      // exit the loop after first iteration in KbCheck mode.
	      if ((numButtons==-1) || ((numButtons==-2) && (keysdown>0))) break;

	      // Sleep for a millisecond before next KbWait loop iteration:
	      PsychWaitIntervalSeconds(0.001);

	    } while(1);

	    if (numButtons==-2) {
	      // KbWait mode: Copy out time value.
	      PsychCopyOutDoubleArg(1, kPsychArgOptional, timestamp);
	    }
	    else {
	      // KbCheck mode:
	      
	      // Copy out overall keystate:
	      PsychCopyOutDoubleArg(1, kPsychArgOptional, (keysdown>0) ? 1 : 0);

	      // Copy out timestamp:
	      PsychCopyOutDoubleArg(2, kPsychArgOptional, timestamp);	      

	      // Copy out keyboard state:
	      PsychAllocOutBooleanMatArg(3, kPsychArgOptional, 1, 256, 1, &buttonStates);

	      // Build 256 elements return vector:
	      for(i=0; i<255; i++) {
		  		buttonStates[i] = (PsychNativeBooleanType)((keyState[i+1]) ? 1 : 0);
	      }
			// Special case: Null out last element:
			buttonStates[255] = (PsychNativeBooleanType) 0;
	    }
	  }
	}
#endif
	
#if PSYCH_SYSTEM == PSYCH_LINUX
	unsigned char keys_return[32];
	char* keystring;
	PsychGenericScriptType *kbNames;
	CGDirectDisplayID dpy;
	Window rootwin, childwin;
	int i, j, mx, my, dx, dy;
	unsigned int mask_return;
	double numButtons, timestamp;
	double* buttonArray;
	PsychNativeBooleanType* buttonStates;
	int keysdown;
	XEvent event_return;
	XKeyPressedEvent keypressevent;
	int screenNumber;

	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	PsychCopyInDoubleArg(1, kPsychArgRequired, &numButtons);

	// Retrieve optional screenNumber argument:
	screenNumber = 0;
	PsychCopyInScreenNumberArg(2, FALSE, &screenNumber);

	// Map screenNumber to X11 display handle and screenid:
	PsychGetCGDisplayIDFromScreenNumber(&dpy, screenNumber);

	// Are we operating in 'GetMouseHelper' mode? numButtons>=0 indicates this.
	if (numButtons>=0) {
	  // Mouse pointer query mode:
	  XQueryPointer(dpy, RootWindow(dpy, PsychGetXScreenIdForScreen(screenNumber)), &rootwin, &childwin, &mx, &my, &dx, &dy, &mask_return);
	  
	  // Copy out mouse x and y position:
	  PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) mx);
	  PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) my);
	  
	  // Copy out mouse button state:
	  PsychAllocOutDoubleMatArg(3, kPsychArgOptional, (int)1, (int)numButtons, (int)1, &buttonArray);

	  // Bits 8, 9 and 10 of mask_return seem to correspond to mouse buttons
	  // 1, 2 and 3 of a mouse for some weird reason. Bits 0-7 describe keyboard modifier keys
	  // like Alt, Ctrl, Shift, ScrollLock, NumLock, CapsLock...
	  // We remap here, so the first three returned entries correspond to the mouse buttons and
	  // the rest is attached behind, if requested...
	  
	  // Mouse buttons: Left, Middle, Right == 0, 1, 2, aka 1,2,3 in Matlab space...
	  for (i=0; i<numButtons && i<3; i++) {
	    buttonArray[i] = (mask_return & (1<<(i+8))) ? 1 : 0; 
	  }
	  // Modifier keys 0 to 7 appended:
	  for (i=3; i<numButtons && i<3+8; i++) {
	    buttonArray[i] = (mask_return & (1<<(i-3))) ? 1 : 0; 
	  }
	  // Everything else appended:
	  for (i=11; i<numButtons; i++) {
	    buttonArray[i] = (mask_return & (1<<i)) ? 1 : 0; 
	  }
	}
	else {
	  // 'KeyboardHelper' mode: We implement either KbCheck() or KbWait() via X11.
	  // This is a hack to provide keyboard queries until a PsychHID() implementation
	  // for Linux is available...

	  if (numButtons==-1 || numButtons==-2) {
	    // KbCheck()/KbWait() mode:

	    // Switch X-Server into synchronous mode: We need this to get
	    // a higher timing precision.
	    XSynchronize(dpy, TRUE);

	    do {
	      // Reset overall key state to "none pressed":
	      keysdown=0;

	      // Request current keyboard state from X-Server:
	      XQueryKeymap(dpy, keys_return);

	      // Request current time of query:
	      PsychGetAdjustedPrecisionTimerSeconds(&timestamp);

	      // Any key down?
	      for (i=0; i<32; i++) keysdown+=(unsigned int) keys_return[i];
	      
	      // We repeat until any key pressed if in KbWait() mode, otherwise we
	      // exit the loop after first iteration in KbCheck mode.
	      if ((numButtons==-1) || ((numButtons==-2) && (keysdown>0))) break;

	      // Sleep for a few milliseconds before next KbWait loop iteration:
	      PsychWaitIntervalSeconds(0.01);
	    } while(1);

	    if (numButtons==-2) {
	      // Copy out time:
	      PsychCopyOutDoubleArg(1, kPsychArgOptional, timestamp);
	    }
	    else {
	      // KbCheck mode:
	      
	      // Copy out overall keystate:
	      PsychCopyOutDoubleArg(1, kPsychArgOptional, (keysdown>0) ? 1 : 0);
	      // copy out timestamp:
	      PsychCopyOutDoubleArg(2, kPsychArgOptional, timestamp);	      
	      // Copy keyboard state:
	      PsychAllocOutBooleanMatArg(3, kPsychArgOptional, 1, 256, 1, &buttonStates);

	      // Map 32 times 8 bitvector to 256 element return vector:
	      for(i=0; i<32; i++) {
				for(j=0; j<8; j++) {
		  			buttonStates[i*8 + j] = (PsychNativeBooleanType)(keys_return[i] & (1<<j)) ? 1 : 0;
				}
	      }
	    }
	  }
	  else if (numButtons == -3) {
	    // numButtons == -3 --> KbName mapping mode:
	    // Return the full keyboard keycode to ASCII character code mapping table...
	    PsychAllocOutCellVector(1, kPsychArgOptional, 256, &kbNames);

	    for(i=0; i<256; i++) {
	      // Map keyboard scan code to KeySym:
	      keystring = XKeysymToString(XKeycodeToKeysym(dpy, i, 0));
	      if (keystring) {
		// Character found: Return its ASCII name string:
		PsychSetCellVectorStringElement(i, keystring, kbNames);
	      }
	      else {
		// No character for this keycode:
		PsychSetCellVectorStringElement(i, "", kbNames);
	      }
	    }
	  }
	  else if (numButtons == -4) {
	    // GetChar() emulation.

/* 	    do { */
/* 	      // Fetch next keypress event from queue, block if none is available... */
/* 	      keystring = NULL; */
/* 	      XNextEvent(dpy, &event_return); */
/* 	      // Check for valid keypress event and extract character: */
/* 	      if (event_return.type == KeyPress) { */
/* 		keypressevent = (XKeyPressedEvent) event_return; */
/* 		keystring = NULL; */
/* 		keystring = XKeysymToString(XKeycodeToKeysym(dpy, keypressevent.keycode, 0)); */
/* 	      } */
/* 	      // Repeat until a valid char is returned. */
/* 	    } while (keystring == NULL); */

/* 	    // Copy out character: */
/* 	    PsychCopyOutCharArg(1, kPsychArgOptional, (char) keystring); */
/* 	    // Copy out time: */
/* 	    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) keypressevent.time); */
	  }
	}
#endif
	return(PsychError_none);	
}

