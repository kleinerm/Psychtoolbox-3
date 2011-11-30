/*
	Psychtoolbox3/Source/Common/Screen/SCREENLoadCLUT.c		

	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
		mario.kleiner@tuebingen.mpg.de  mk
  
	PLATFORMS:	

	All.

	HISTORY:

		4/19/06  mk		Created.

	DESCRIPTION:

	Interface to the operating systems hardware gamma-table functions. This is just
	a wrapper around PsychLoadNormalizedGammaTable() for backwards compatibility.
	New code should use the SCREENLoadNormalizedGammaTable function.

	TO DO:  

*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "oldclut = Screen('LoadCLUT', ScreenNumber [, clut] [, startEntry=0] [, bits=8]);";

static char synopsisString[] = 
"Load or query the hardware gamma table of the specified screen. If you pass a new 'clut' "
"hardware gamma table, then 'clut' needs to be a matrix with 1 to (256-startEntry) rows and 3 columns. "
"Each row corresponds to a single color index value  and contains the Red- green- and blue values to use for "
"output of a pixel of that color. If you provide 'startEntry' then the hardware clut is overwritten by your table starting at "
"color index 'startEntry' instead of starting at index 0. You are free to provide a clut with only a few rows if "
"you only want to update a subset of the color indices in the hardware clut. Column 1 is the red value, column 2 "
"is the green value and column 3 is the blue value that you want to load into the hardware clut for a specific index. "
"Values in the table have to be in range between 0 (for dark pixel) and 'max' (for maximum DAC output intensity), "
"where 'max' depends on the number of bits that the RAMDAC of your graphics hardware supports. Psychtoolbox "
"currently has no way of knowing the bit-resolution of your RAMDAC, so you have to provide it in the optional "
"argument 'bits'. If you don't provide a 'bits' setting, it defaults to a safe default of 8-Bits. Some examples "
"for 'bits' vs. 'max': 8 bits (default) --> max = 255. 9 bits --> max=511. 10 bits --> max=1023. 12 bits --> max = 4095. "
"On MacOS-X and Linux, this function takes arbitrary clut-tables which makes it suitable for fast CLUT animation. "
"On Microsoft Windows, only tables with monotonically increasing values are considered valid. Other tables "
"get rejected by the operating system -- there's nothing we can do about this incredibly wise decision "
"of the Microsoft system designers :( , so this function is not suitable for CLUT animation, but only for linearizing "
"or calibrating display devices. PLEASE NOTE: LoadCLUT is only provided to keep old code from OS-9 PTB and old "
"Windows PTB working. It is just a wrapper around 'LoadNormalizedGammaTable'. Use 'LoadNormalizedGammaTable' for "
"new code. That function is independent of DAC resolution as it takes values in a normalized range between 0.0 and 1.0 "
"and always gives you the highest possible resolution for your gamma table, despite not knowing the real DAC resolution. "
"This function returns the old CLUT as optional return argument. If you don't pass a clut, then it only returns the old CLUT.";

static char seeAlsoString[] = "LoadNormalizedGammaTable ReadNormalizedGammaTable";

PsychError SCREENLoadCLUT(void) 
{
    int		i, step, screenNumber, numEntries, inM, inN, inP, start, bits;
    float 	*outRedTable, *outGreenTable, *outBlueTable, *inRedTable, *inGreenTable, *inBlueTable;
    double	 *inTable, *outTable, maxval;	
    psych_bool     isclutprovided;

    start = 0;
    bits = 8;

    //all subfunctions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(4));

    // Read in the screen number:
    PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
    
    // Read in optional start index:
    PsychCopyInIntegerArg(3, FALSE, &start);
    if (start<0 || start>255) {
      PsychErrorExitMsg(PsychError_user, "Argument startEntry must be between zero and 255.");
    }

    // Read in optional bits argument:
    PsychCopyInIntegerArg(4, FALSE, &bits);
    if (bits<1 || bits>16) {
      PsychErrorExitMsg(PsychError_user, "Argument 'bits' must be between 1 and 16.");
    }

    // Compute allowable maxval:
    maxval=(double) ((1 << bits) - 1);

    // First read the existing gamma table so we can return it.  
    PsychReadNormalizedGammaTable(screenNumber, -1, &numEntries, &outRedTable, &outGreenTable, &outBlueTable);

    // Load and sanity check the input matrix, and convert from float to doubles:
    isclutprovided = PsychAllocInDoubleMatArg(2, FALSE, &inM,  &inN, &inP, &inTable);

    inRedTable = PsychMallocTemp(sizeof(float) * 256);
    inGreenTable = PsychMallocTemp(sizeof(float) * 256);
    inBlueTable = PsychMallocTemp(sizeof(float) * 256);

    // Copy the table, subsample it if it is bigger than 256 slots:
    if (numEntries < 256) PsychErrorExitMsg(PsychError_system, "Returned hardware gamma table has less than 256 slots?!? I can not handle this!");
    step = numEntries / 256;
    for(i = 0; i < 256; i++) {
      inRedTable[i]   = outRedTable[i * step];
      inGreenTable[i] = outGreenTable[i * step];
      inBlueTable[i]  = outBlueTable[i * step];
    }

    // Allocate output array:
    PsychAllocOutDoubleMatArg(1, FALSE, 256, 3, 0, &outTable);

    // Copy read table into output array, scale it by maxval to map range 0.0-1.0 to 0-maxval:
    for(i = 0; i < 256; i++){
      outTable[PsychIndexElementFrom3DArray(256, 3, 0, i, 0, 0)]=(double) inRedTable[i] * maxval;
      outTable[PsychIndexElementFrom3DArray(256, 3, 0, i, 1, 0)]=(double) inGreenTable[i] * maxval;
      outTable[PsychIndexElementFrom3DArray(256, 3, 0, i, 2, 0)]=(double) inBlueTable[i] * maxval;
    }

    if (isclutprovided) {
      // Sanity check:
      if((inM > 256 - start) || (inM < 1) || (inN != 3) || (inP != 1))
        PsychErrorExitMsg(PsychError_user, "The provided CLUT table must have a size between 1 and (256 - startEntry) rows and 3 columns.");

      // Now we can overwrite entries 'start' to start+inM of inTable with the user provided table values. We
      // need to scale the users values down from 0-maxval to 0.0-1.0:
      for(i = start; (i < 256) && (i-start < inM); i++){
	inRedTable[i]   = (float) (inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i-start, 0, 0)] / maxval);
	inGreenTable[i] = (float) (inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i-start, 1, 0)] / maxval);
	inBlueTable[i]  = (float) (inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i-start, 2, 0)] / maxval);
	
	// Range check:
	if(inRedTable[i]>1 || inRedTable[i]< 0 || inGreenTable[i] > 1 || inGreenTable[i] < 0 || inBlueTable[i] >1 || inBlueTable[i] < 0) {
	  printf("PTB-ERROR: At least one of the CLUT values in row %i is outside the valid range of 0 to %i!\n", i-start+1, ((1 << bits) - 1));
	  PsychErrorExitMsg(PsychError_user, "Tried to set a CLUT with invalid entries.");
	}
      }
      
      // Now set the new gamma table
      PsychLoadNormalizedGammaTable(screenNumber, -1, 256, inRedTable, inGreenTable, inBlueTable);
    }

    return(PsychError_none);
}
