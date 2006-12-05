/*

  Psychtoolbox3/Source/Common/SCREENGamma.c		

  

  AUTHORS:

  Allen.Ingling@nyu.edu		awi 

  

  PLATFORMS:	

  This file should build on any platform. 



  HISTORY:

  2/02/03  awi		Created. 

 

  DESCRIPTION:

  

  Sets and retrieves gamma. 

  

  TO DO:

  

  This only supports 8-bits DACS.  Core Graphics seems to provide no way to discover whether 

  

*/





#include "Screen.h"





static char useString[]= "[oldGammaTable,oldBits,gammaError]=Screen('Gamma', windowPtrOrScreenNumber,[gammaTable],[bits])";

static char synopsisString[] = 

        "Sets and reads gamma";

static char seeAlsoString[] = "";

	 



PsychError SCREENGamma(void) 

{

    int		i, screenNumber, numEntries;

    float 	*redTable, *greenTable, *blueTable;

    double	*gammaTable;	

	

    //all subfunctions should have these two lines

    PsychPushHelp(useString, synopsisString, seeAlsoString);

    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};



    PsychErrorExit(PsychCapNumOutputArgs(3));

    PsychErrorExit(PsychCapNumInputArgs(3));

    

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










