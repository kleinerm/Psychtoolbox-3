/*
	PsychToolbox3/Source/Common/GetSecsMex/GetSecsMex.h
	
	PLATFORMS:	Matlab only
			
	
	AUTHORS:
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
		8/06/02			awi		wrote it. 

	DESCRIPTION 

		GetSecsMex is a variant of GetSecs which strips out the Psychtoolbox mex/mx abstraction layer.
		The purpsoe is to provide a simplified example to Mathworks and verify that the psychtoolbox mex/mx abstraction layer
		is not the source of trouble.  


*/


#include "GetSecsMex.h"

void PsychGetPrecisionTimerSeconds(double *secs)
{
    UInt64	ticks;
    
    ticks=AudioGetCurrentHostTime();
    *secs=(double)(AudioConvertHostTimeToNanos(ticks) / (double)1000000000.0);
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    
    if(nlhs > 1)
        mexErrMsgTxt("GetSecsMex uses no more than one output argument.");
    if(nrhs > 0)
        mexErrMsgTxt("GetSecsMex uses no input arguments.");
    plhs[0]=mxCreateDoubleMatrix(1, 1, mxREAL);   
    PsychGetPrecisionTimerSeconds(mxGetPr(plhs[0]));
    
}




	
