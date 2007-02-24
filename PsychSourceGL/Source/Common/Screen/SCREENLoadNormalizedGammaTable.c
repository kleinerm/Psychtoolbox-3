/*
	Psychtoolbox3/Source/Common/SCREENLoadNormalizedGammaTable.c		

	AUTHORS:

	Allen.Ingling@nyu.edu		awi 
	mario.kleiner@tuebingen.mpg.de  mk
  
	PLATFORMS:	

	All.

	HISTORY:
	
	1/27/03  awi		Created. 
	1/30/06  mk       Improved online help text.
 
	DESCRIPTION:

	Interface to the operating systems hardware gamma-table functions.    

	TO DO:  
*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('LoadNormalizedGammaTable', ScreenNumber, table [, loadOnNextFlip]);";
static char synopsisString[] = 
			"Load the gamma table of the specified screen. You need to pass the new "
			"hardware gamma table 'table' as a 256 rows by 3 columns matrix. Each row corresponds to "
			"a single color index value in the framebuffer and contains the Red- green- and blue values "
			"to use for output. Column 1 is the red value, column 2 is the green value and column 3 is "
			"the blue value. Values have to be in range between 0.0 (for dark pixel) and 1.0 (for maximum intensity). "
			"Example: table(127,1)=0.67 would mean that the red color value 127 should be displayed with 67% of "
			"the maximum red-gun intensity, table(32, 3)=0.11 means that blue color value 32 should be displayed "
			"with 11% of the maximum blue-gun intensity. The range of values 0-1 gets mapped to the hardware with "
			"the accuracy attainable by the hardwares DAC's, typically between 8 and 10 bits. "
			"On OS-X you can also pass 512, 1024, 2048, ..., 65535 rows instead of 256 rows, although this only "
			"makes sense for a few selected applications, e.g., setup for the Bits++ box. "
			"If you provide the index of an onscreen window as 'ScreenNumber' and you set the (optional) "
			"flag 'loadOnNextFlip' to 1, then update of the gamma table will not happen immediately, but only at "
			"execution of the Screen('Flip', windowPtrOrScreenNumber) command. This allows to synchronize change of "
			"both the visual stimulus and change of the gamma table with each other and to the vertical retrace. If "
			"the flag is set to its default value of zero then update of the gamma table will happen at the next "
			"vertical retrace (or immediately if the graphics driver doesn't support sync to vertical retrace). "
			"On MacOS-X, this function takes arbitrary gamma-tables which makes it suitable for fast CLUT animation. "
			"On Microsoft Windows, only tables with monotonically increasing values are considered valid. Other tables "
			"get rejected by the operating system -- there's nothing we can do about this incredibly wise decision "
			"of the Microsoft system designers :( , so this is not suitable for CLUT animation, but only for linearizing "
			"or calibrating display devices. "
			"The function returns the old gamma table as optional return argument. ";

static char seeAlsoString[] = "ReadNormalizedGammaTable";

PsychError SCREENLoadNormalizedGammaTable(void) 
{
    int		i, screenNumber, numEntries, inM, inN, inP, loadOnNextFlip;
    float 	*outRedTable, *outGreenTable, *outBlueTable, *inRedTable, *inGreenTable, *inBlueTable;
    double	 *inTable, *outTable;	
	 PsychWindowRecordType		*windowRecord;

    //all subfunctions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(3));

    //read in the screen number
    PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
    
    //load, sanity check the input matrix, and covert from float to doubles
    PsychAllocInDoubleMatArg(2, TRUE, &inM,  &inN, &inP, &inTable);

    if((inN != 3) || (inP != 1)) PsychErrorExitMsg(PsychError_user, "The gamma table must have 3 columns (Red, Green, Blue).");
	
	#if PSYCH_SYSTEM == PSYCH_OSX
		// OS-X allows tables with other than 256 slots. It either passes them to hw if in native size, or performs
		// software interpolation to convert it into native size:
		if((inM != 256) && (inM != 512) && (inM != 1024) && (inM != 2048) && (inM != 4096) && (inM != 8192) && (inM != 16384) && (inM != 32768) && (inM != 65535)) {
			PsychErrorExitMsg(PsychError_user, "The gamma table must have a number of rows equal to one of these: 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 or 65535.");
		}
	#else
		// Windows requires 256 slots, i didn't check for Linux yet, but this is always safe, so...
		if(inM != 256) {
			PsychErrorExitMsg(PsychError_user, "The gamma table must have 256 rows.");
		}
	#endif
	
	 // Copy in optional loadOnNextFlip - flag. It defaults to zero. If provided
	 // with a non-zero value, we will defer actual update of the gamma table to
	 // the next bufferswap as initiated via Screen('Flip').
	 loadOnNextFlip = 0;
	 PsychCopyInIntegerArg(3, FALSE, &loadOnNextFlip);

	 if (loadOnNextFlip>0) {
		 // Allocate tables in associated windowRecord: We will update during next
		 // Flip operation for specified windowRecord.
		 PsychAllocInWindowRecordArg(1, TRUE, &windowRecord);
		 
		 // Sanity checks:
		 if (!PsychIsOnscreenWindow(windowRecord)) PsychErrorExitMsg(PsychError_user, "Target window for gamma table upload is not an onscreen window!");
		 if (windowRecord->inRedTable && loadOnNextFlip!=2) PsychErrorExitMsg(PsychError_user, "This window has already a new gamma table assigned for upload on next Flip!");
		 
		 if (windowRecord->inRedTable && windowRecord->inTableSize != inM) {
			free(windowRecord->inRedTable); windowRecord->inRedTable = NULL;
			free(windowRecord->inGreenTable); windowRecord->inGreenTable = NULL;
			free(windowRecord->inBlueTable); windowRecord->inBlueTable = NULL;
		 }
		 
		 if (windowRecord->inRedTable == NULL) {
			 // Allocate persistent memory:
			 inRedTable=malloc(sizeof(float) * inM);
			 inGreenTable=malloc(sizeof(float) * inM);
			 inBlueTable=malloc(sizeof(float) * inM);
			 
			 // Assign the pointers to the windowRecord:
			 windowRecord->inRedTable = inRedTable;
			 windowRecord->inGreenTable = inGreenTable;
			 windowRecord->inBlueTable = inBlueTable;
			 windowRecord->inTableSize = inM;
		 }
		 else {
			inRedTable = windowRecord->inRedTable;
			inGreenTable = windowRecord->inGreenTable;
			inBlueTable = windowRecord->inBlueTable;
		 }
		
		 windowRecord->loadGammaTableOnNextFlip = (loadOnNextFlip == 1) ? 1 : 0;
	 }
	 else {
		 // Allocate temporary tables: We will update immediately.
		 inRedTable=PsychMallocTemp(sizeof(float) * inM);
		 inGreenTable=PsychMallocTemp(sizeof(float) * inM);
		 inBlueTable=PsychMallocTemp(sizeof(float) * inM);
	 }
	 
    for(i=0;i<inM;i++){
        inRedTable[i]=(float)inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i, 0, 0)];
        inGreenTable[i]=(float)inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i, 1, 0)];
        inBlueTable[i]=(float)inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i, 2, 0)];

        if(inRedTable[i]>1 || inRedTable[i]< 0 || inGreenTable[i] > 1 || inGreenTable[i] < 0 || inBlueTable[i] >1 || inBlueTable[i] < 0)
            PsychErrorExitMsg(PsychError_user, "Gamma Table Values must be in interval 0 =< x =< 1");
    }

    //first read the existing gamma table so we can return it.  
    PsychReadNormalizedGammaTable(screenNumber, &numEntries, &outRedTable, &outGreenTable, &outBlueTable);
    PsychAllocOutDoubleMatArg(1, FALSE, numEntries, 3, 0, &outTable);

    for(i=0;i<numEntries;i++){
        outTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 0, 0)]=(double)outRedTable[i];
        outTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 1, 0)]=(double)outGreenTable[i];
        outTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 2, 0)]=(double)outBlueTable[i];
    }

    //Now set the new gamma table
    if (loadOnNextFlip == 0) PsychLoadNormalizedGammaTable(screenNumber, inM, inRedTable, inGreenTable, inBlueTable);

    return(PsychError_none);
}

