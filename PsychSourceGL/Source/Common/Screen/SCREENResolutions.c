/*
  Psychtoolbox3/Source/Common/Screen/SCREENResolutions.c		
  
  AUTHORS:

  mario.kleiner at tuebingen.mpg.de  mk
  
  PLATFORMS:	

  This file should build on any platform. 

  HISTORY:
  9/23/07  mk		Created. 
 
  DESCRIPTION:
  
  Contains the two subfunctions 'Resolutions' and 'Resolution'.
  Enumerates and returns all supported display modes - combinations of resolution, refresh rate and color depth.
  Allows to query and set display mode.
  
  TO DO:

*/

#include "Screen.h"

const char *FieldNames[]={"width", "height", "pixelSize", "hz"};

PsychError SCREENResolution(void) 
{
	static char useString[] = "oldResolution=Screen('Resolution', screenNumber [, newwidth] [, newheight] [, newHz] [, newPixelSize] [, specialMode]);";
	//															  1				2				3			4			5			    6
	static char synopsisString[] =	"Query or change display settings for screen \"screenNumber\".\n"
									"Returns a struct \"oldResolutions\" with the current settings for screen "
									"resolution, refresh rate and pixel depth. Optionally sets new settings "
									"for screen resolution \"newwidth\" x \"newheight\", refresh rate \"newHz\" "
									"and framebuffer pixel depth \"newPixelSize\". Providing invalid or incompatible "
									"settings will raise an error. Especially the color depth \"newPixelSize\" should "
									"usually not be set to anything else than 32 bpp. A setting of 24 bpp can impair "
									"alpha-blending on some systems, a setting of 16 bpp will disable alpha-blending "
									"and create drastically reduced color resolution of 5 bits per color channel. A "
									"setting of 8 bpp is not supported at all on MacOS/X and will create artifacts "
									"on all other systems. Use a size of 32 bpp even for clut animation. This function "
									"may not work on all MS-Windows or GNU/Linux setups, your mileage may vary...\n"
									"Psychtoolbox will automatically restore the systems display resolution to the "
									"system settings made via the display control panel as soon as either your script "
									"finishes by closing all its windows or by some error. Terminating Matlab due to "
									"quit command or due to crash will also restore the system preference settings. "
									"If you call this command without ever opening onscreen windows and closing them "
									"at some point, Psychtoolbox will not restore display settings automatically.\n"
									"You can query a list of all supported combinations of display settings via the "
									"Screen('Resolutions') command. \"specialMode\" is a flag you must not touch, "
									"unless you really know what you're doing, that's why we don't tell you its purpose.";
									
	static char seeAlsoString[] = "Screen('Resolutions')";
	
    PsychGenericScriptType	*oldResStructArray;
	PsychScreenSettingsType screenSettings;
	PsychDepthType			useDepth;
    int						screenNumber, specialMode;
	long					newWidth, newHeight, newHz, newBpp;
	boolean					rc;
	
	// Purpose of 'specialMode': If bit zero is set, then its possible to switch display settings while
	// onscreen windows - possibly fullscreen windows and display capture - are open/active. This is mostly
	// useful for temporarilly changing display framerate, e.g., from a wanted value of x Hz to a intermittent
	// value of y Hz, then back to x Hz. Such an approach may be useful for "drift-syncing" multiple displays
	// whose video refresh cycles are not perfectly in sync (not yet clear if this will really work), but that's
	// about the only useful purpose. Changing other settings - or changing any settings permanently - while
	// windows are already open will likely subvert display calibration and lead to severe timing problems
	// and possible worse things - like complete hangs of the graphics subsystem and the need for a hard
	// machine reset!
	// Other bits of specialMode will trigger other sync related actions - implementation and semantics may
	// change without notice!
	
    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(6));
    
	// Get the screen number from the windowPtrOrScreenNumber.  This also checks to make sure that the specified screen exists.  
    PsychCopyInScreenNumberArg(kPsychUseDefaultArgPosition, TRUE, &screenNumber);
    if(screenNumber==-1) PsychErrorExitMsg(PsychError_user, "The specified onscreen window has no ancestral screen or invalid screen number."); 

    // Create a structure and populate it.
    PsychAllocOutStructArray(1, FALSE, 1, 4, FieldNames, &oldResStructArray);

	// Query and return resolution:
	PsychGetScreenSize(screenNumber, &newWidth, &newHeight);
    PsychSetStructArrayDoubleElement("width", 0, (int) newWidth, oldResStructArray);
    PsychSetStructArrayDoubleElement("height", 0, (int) newHeight, oldResStructArray);

	// Query and return refresh rate:
	newHz = (int) PsychGetNominalFramerate(screenNumber);
    PsychSetStructArrayDoubleElement("hz", 0, newHz, oldResStructArray);
	
	// Query and return current display depth:
	newBpp = PsychGetScreenDepthValue(screenNumber);
    PsychSetStructArrayDoubleElement("pixelSize", 0, newBpp, oldResStructArray);

	// Any new settings provided? Otherwise we skip this:
	if(PsychGetNumInputArgs() > 1) {
		// Get optional specialMode flag, default to zero:
		specialMode = 0;
		PsychCopyInIntegerArg(6, FALSE, &specialMode);

		// Make sure we're not called while windows are open. Only allow calling with open windows if Bit zero of specialMode is set:
		if ((PsychCountOpenWindows(kPsychAnyWindow) > 0) && !(specialMode & 1)) PsychErrorExitMsg(PsychError_user, "Tried to change video display settings via Screen('Resolutions'); while onscreen windows were open! Not allowed.");

		// Copy in optional new settings:
		PsychCopyInIntegerArg(2, FALSE, &newWidth);
		PsychCopyInIntegerArg(3, FALSE, &newHeight);
		PsychCopyInIntegerArg(4, FALSE, &newHz);
		PsychCopyInDepthValueArg(5, FALSE, &newBpp);
		PsychInitDepthStruct(&useDepth);
		PsychAddValueToDepthStruct(newBpp, &useDepth);
		
		// Switch to new display mode, according to these specs:
		PsychGetScreenSettings(screenNumber, &screenSettings);
		screenSettings.rect[kPsychLeft] = 0;
		screenSettings.rect[kPsychTop] = 0;
		screenSettings.rect[kPsychRight] = newWidth;
		screenSettings.rect[kPsychBottom] = newHeight;
		screenSettings.nominalFrameRate = newHz;
		PsychInitDepthStruct(&(screenSettings.depth));
		PsychCopyDepthStruct(&(screenSettings.depth), &useDepth);

		// Perform sanity check:
		if (!PsychCheckVideoSettings(&screenSettings)) PsychErrorExitMsg(PsychError_user, "Invalid or mutually incompatible video settings requested!\nOne or more of the values are invalid or unsupported by your display device.");
		
		// Perform actual switch:
        if (!(specialMode & 1)) PsychCaptureScreen(screenNumber);
		rc = PsychSetScreenSettings( TRUE, &screenSettings);
        if (!(specialMode & 1)) PsychReleaseScreen(screenNumber);

		// Check if successfull:
		if (!rc) PsychErrorExitMsg(PsychError_user, "Invalid or mutually incompatible video settings requested!\nOne or more of the values are invalid or unsupported by your display device.");
	}

    return(PsychError_none);	
}

PsychError SCREENResolutions(void) 
{
	static char useString[] = "resolutions = Screen('Resolutions', screenNumber);";
	static char synopsisString[] =	"Query a list of all supported and valid display settings for screen "
									"\"screenNumber\". You can set your display to one of the supported "
									"combinations of settings via the Screen('Resolution') command.\n"
									"Returns an array of Matlab/Octave structs \"resolutions\", where "
									"each element in the array is a struct that describes one valid "
									"combination of resolution, color depth and refresh rate. Fields are "
									"self explanatory.\n"
									"Please note that unless you have good reason to do so, especially the "
									"color depth value \"newPixelSize\" should usually not be set to anything "
									"else than 32 bpp. A setting of 24 bpp can impair alpha-blending on some "
									"systems, a setting of 16 bpp will disable alpha-blending and create "
									"drastically reduced color resolution of 5 bits per color channel. A "
									"setting of 8 bpp is not supported at all on MacOS/X and will create artifacts "
									"on all other systems. Use a size of 32 bpp even for clut animation. This function "
									"may not work on all MS-Windows or GNU/Linux setups, your mileage may vary.\n"
									"Please note that there are a couple of helper functions in the PsychOneLiners "
									"directory of Psychtoolbox which can simplify the task of switching resolutions. "
									"That functions are probably more convenient to use than the low-level Screen "
									"functions for display settings... ";
	static char seeAlsoString[] = "Screen('Resolution')";
	
    PsychGenericScriptType	*ResStructArray;
    int						screenNumber, nrmodes, i;
	long					*widths, *heights, *hz, *bpp;
	
    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
	// Get the screen number from the windowPtrOrScreenNumber.  This also checks to make sure that the specified screen exists.  
    PsychCopyInScreenNumberArg(kPsychUseDefaultArgPosition, TRUE, &screenNumber);
    if(screenNumber==-1) PsychErrorExitMsg(PsychError_user, "The specified onscreen window has no ancestral screen or invalid screen number."); 

	// Query all supported settings:
	nrmodes = PsychGetAllSupportedScreenSettings(screenNumber, &widths, &heights, &hz, &bpp);
	
    // Create a structure array and populate it.
    PsychAllocOutStructArray(1, FALSE, nrmodes, 4, FieldNames, &ResStructArray);

	for (i=0; i<nrmodes; i++) {
		PsychSetStructArrayDoubleElement("width", i, (double) widths[i], ResStructArray);
		PsychSetStructArrayDoubleElement("height", i, (double) heights[i], ResStructArray);
		PsychSetStructArrayDoubleElement("hz", i, (double) hz[i], ResStructArray);
		PsychSetStructArrayDoubleElement("pixelSize", i, (double) bpp[i], ResStructArray);
	}
	
	// Ready.
    return(PsychError_none);	
}
