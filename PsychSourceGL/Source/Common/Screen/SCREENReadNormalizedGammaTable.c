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
static char useString[] = "Screen('ReadNormalizedGammaTable', ScreenNumber);";
static char synopsisString[] = 
        "Reads the gamma table of the specified screen. See help for Screen('LoadNormalizedGammaTable'); "
        "for infos about the format of the returned table and for further explanations regarding gamma tables.";

static char seeAlsoString[] = "";

PsychError SCREENReadNormalizedGammaTable(void)
{
    int		i, screenNumber, numEntries;
    float 	*redTable, *greenTable, *blueTable;
    double	*gammaTable;	

    //all subfunctions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
    PsychReadNormalizedGammaTable(screenNumber, &numEntries, &redTable, &greenTable, &blueTable);
    PsychAllocOutDoubleMatArg(1, FALSE, numEntries, 3, 0, &gammaTable);

    for(i=0;i<numEntries;i++){
        gammaTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 0, 0)]=(double)redTable[i];
        gammaTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 1, 0)]=(double)greenTable[i];
        gammaTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 2, 0)]=(double)blueTable[i];
    }

    return(PsychError_none);
}

