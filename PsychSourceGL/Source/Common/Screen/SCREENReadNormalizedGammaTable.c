/*
	Psychtoolbox3/Source/Common/SCREENReadNormalizedGammaTable.c		

	AUTHORS:

	Allen.Ingling@nyu.edu		awi 
	mario.kleiner@tuebingen.mpg.de  mk
	PLATFORMS:	

	All.

	HISTORY:

	1/27/03		awi		Created. 

	DESCRIPTION:

	TO DO:
*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[gammatable, dacbits, reallutsize] = Screen('ReadNormalizedGammaTable', ScreenNumber);";
static char synopsisString[] = 
        "Reads and returns the gamma table 'gammatable' of the specified screen. Returns the output resolution "
		"of the video DAC as optional second argument 'dacbits'. Will return dacbits=8 as a safe default if it "
		"is unable to query the real resolution of the DAC. Currently only OS-X reports the real DAC size. "
		"Will return the real number of slots in the hardware lookup table in optional return argument 'reallutsize'. "
		"Currently only OS-X reports the real LUT size. "
		"See help for Screen('LoadNormalizedGammaTable'); for infos about the format of the returned table "
        "and for further explanations regarding gamma tables.";

static char seeAlsoString[] = "";

PsychError SCREENReadNormalizedGammaTable(void)
{
    int		i, screenNumber, numEntries, reallutsize;
    float 	*redTable, *greenTable, *blueTable;
    double	*gammaTable;	

    //all subfunctions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(1));

    PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
    PsychReadNormalizedGammaTable(screenNumber, &numEntries, &redTable, &greenTable, &blueTable);
    PsychAllocOutDoubleMatArg(1, FALSE, numEntries, 3, 0, &gammaTable);

    for(i=0;i<numEntries;i++){
        gammaTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 0, 0)]=(double)redTable[i];
        gammaTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 1, 0)]=(double)greenTable[i];
        gammaTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 2, 0)]=(double)blueTable[i];
    }

	// Copy out optional DAC resolution value:
	PsychCopyOutDoubleArg(2, FALSE, (double) PsychGetDacBitsFromDisplay(screenNumber));
	
	// We default to the assumption that the real size of the hardware LUT is identical to
	// the size of the returned LUT:
	reallutsize = numEntries;
	
	#if PSYCH_SYSTEM == PSYCH_OSX
		// On OS-X we query the real LUT size from the OS and return that value:
		CGDirectDisplayID	displayID;
		CFMutableDictionaryRef properties;
		CFNumberRef cfGammaLength;
		SInt32 lutslotcount;
		io_service_t displayService;
		kern_return_t kr;
		
		// Retrieve display handle for screen:
		PsychGetCGDisplayIDFromScreenNumber(&displayID, screenNumber);
		
		// Retrieve low-level IOKit service port for this display:
		displayService = CGDisplayIOServicePort(displayID);
				
		// Obtain the properties from that service
		kr = IORegistryEntryCreateCFProperties(displayService, &properties, NULL, 0);
		if((kr == kIOReturnSuccess) && ((cfGammaLength = (CFNumberRef) CFDictionaryGetValue(properties, CFSTR(kIOFBGammaCountKey)))!=NULL))
		{
			CFNumberGetValue(cfGammaLength, kCFNumberSInt32Type, &lutslotcount);
			CFRelease(properties);
			reallutsize = (int) lutslotcount;
		}
		else {
			// Failed!
			if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to query real size of video LUT for screen %i! Will return safe default of %i slots.\n", screenNumber, reallutsize);
		}	
	#endif
	
	// Copy out optional real LUT size (number of slots):
	PsychCopyOutDoubleArg(3, FALSE, (double) reallutsize);

    return(PsychError_none);
}

