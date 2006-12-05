/*
	PsychToolbox3/Source/Common/DoNothing/DoNothing.c
	
	PLATFORMS:	Only OS X for now.
	
	AUTHORS:

		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:

		7/05/03			awi		wrote it. 
		7/10/04		
	
	DESCRIPTION:
        
        DoNothing in the OS 9 Psychtoolbox measures Matlab's mex calling overhead.

        DoNothing in the OS X Psychtoolbox measures the combined overhead of Matlabs's mex and the Psychtoolbox "Scripting Glue" which 
		abstracts up Matlab's mex. 

*/


#include "DoNothing.h"


PsychError DONOTHINGDoNothing(void) 
{
    double 	*returnValue;
    int		i, numReturnArguments;	
  
    
    numReturnArguments=PsychGetNumOutputArgs();
    for(i=0;i<numReturnArguments;i++)
        PsychAllocOutDoubleArg(i+1, TRUE, &returnValue);

    return(PsychError_none);	
}


/*


COPYRIGHT:
  Copyright Denis Pelli, 1997.
  This file may be distributed freely as long as this notice accompanies
  it and any changes are noted in the source.  It is distributed as is,
  without any warranty implied or provided.  We accept no liability for
  any damage or loss resulting from the use of this software.
PURPOSE:
  Do nothing. Used to measure Matlab's mex calling overhead.
HISTORY:
8/17/97 dgp	Wrote it.


#include <Psychtoolbox.h>

ROUTINE: mexFunction()

char useDoNothing[]="[empty1,...]=DoNothing([arg1],[arg2],...)";

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{
	long i;

	nrhs;prhs;
	for(i=0;i<nlhs;i++)plhs[i]=mxCreateDoubleMatrix(0,0,mxREAL);
}
*/




	
