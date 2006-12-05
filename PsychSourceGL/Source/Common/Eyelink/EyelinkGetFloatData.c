/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkGetNextDataType.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		29/05/2001	emp 		created it (OS9 version)
		30/10/06	fwc			Adapted from early alpha version.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			PsychHardware/EyelinkToolbox

/* WARNING returns only samples at the moment ??? doesn't look like that */


#include "PsychEyelink.h"

static char useString[] = "item = Eyelink('GetFloatData', type)";

static char synopsisString[] = 
   " makes copy of last item."
   " You have to supply the type (returned from Eyelink('getnextdatatype'))."
   " Returns the item.";

static char seeAlsoString[] = "";
 
/*
ROUTINE: EyelinkGetFloatData.c
PURPOSE:
   uses INT16 CALLTYPE eyelink_get_float_data(void FARTYPE *buf);
   makes copy of last item from getnextdatatype
   You have to supply the type (returned from eyelink('getnextdatatype')).
   Returns the item.*/

PsychError EyelinkGetFloatData(void)
{
   FSAMPLE   fs;
   FEVENT    fe;
   int type = 0;
   mxArray **mxpp;
   	
   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(1));
   PsychErrorExit(PsychRequireNumInputArgs(1));
   PsychErrorExit(PsychCapNumOutputArgs(1));

	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

   PsychCopyInIntegerArg(1, TRUE, &type);
         
   mxpp = PsychGetOutArgMxPtr(1); 

   switch(type)
   {
      case SAMPLE_TYPE:   eyelink_get_float_data(&fs);
                          (*mxpp) = CreateMXFSample(&fs);
                          break;
//      case 0:             (*mxpp) = (mxArray*) mxCreateDoubleMatrixPtr(1,1,mxREAL); //no data
//                          mxGetPrPtr((*mxpp))[0] = 0;
//                          break;
      case 0:             (*mxpp) = (mxArray*) mxCreateDoubleMatrix(1,1,mxREAL); //no data
                          mxGetPr((*mxpp))[0] = 0;
                          break;
      default:            eyelink_get_float_data(&fe);  // else it is an event
                          (*mxpp) = CreateMXFEvent(&fe);
   }   
   
   return(PsychError_none);
}
