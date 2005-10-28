/*

	Psychtoolbox3/Source/Common/SCREENLoadNormalizedGammaTable.c		

  

	AUTHORS:



		Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS:	

	

		Only OS X for now.



	HISTORY:



		1/27/03  awi		Created. 

 

	DESCRIPTION:

  

  

	TO DO:

  

*/





#include "Screen.h"



// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c

static char useString[] = "Screen('LoadNormalizedGammaTable', windowPtrOrScreenNumber, table);";

static char synopsisString[] = 

        "Load the gamma table of the specified screen";

static char seeAlsoString[] = "ReadNormalizedGammaTable";

	 



PsychError SCREENLoadNormalizedGammaTable(void) 

{

    int		i, screenNumber, numEntries, inM, inN, inP;

    float 	*outRedTable, *outGreenTable, *outBlueTable, *inRedTable, *inGreenTable, *inBlueTable;

    double	 *inTable, *outTable;	

	

    //all subfunctions should have these two lines

    PsychPushHelp(useString, synopsisString, seeAlsoString);

    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};



    PsychErrorExit(PsychCapNumOutputArgs(1));

    PsychErrorExit(PsychCapNumInputArgs(2));

    

    //read in the screen number

    PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);

    

    //load, sanity check the input matrix, and covert from float to doubles

    PsychAllocInDoubleMatArg(2, TRUE, &inM,  &inN, &inP, &inTable);

    if((inM != 256) || (inN != 3) || (inP != 1))

        PsychErrorExitMsg(PsychError_user, "The gamma table must be 256x3");

    inRedTable=mxMalloc(sizeof(float) * 256);

    inGreenTable=mxMalloc(sizeof(float) * 256);

    inBlueTable=mxMalloc(sizeof(float) * 256);

    for(i=0;i<256;i++){

        inRedTable[i]=(float)inTable[PsychIndexElementFrom3DArray(256, 3, 0, i, 0, 0)];

        inGreenTable[i]=(float)inTable[PsychIndexElementFrom3DArray(256, 3, 0, i, 1, 0)];

        inBlueTable[i]=(float)inTable[PsychIndexElementFrom3DArray(256, 3, 0, i, 2, 0)];

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

    PsychLoadNormalizedGammaTable(screenNumber, numEntries, inRedTable, inGreenTable, inBlueTable);



    return(PsychError_none);	

}













