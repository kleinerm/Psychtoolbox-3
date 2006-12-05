/*
	PsychToolbox3/Source/Common/WaitSecsMex/WaitSecsMex.c
	
	PLATFORMS:	Matlab only
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	8/06/02			awi		wrote it. 
        
        DESCRIPTION 

        WaitSecsMex is a variant of WaitSecs which strips out the Psychtoolbox mex/mx abstraction layer.
        The purposes are to provide a simplified example to Mathworks and to verify that the psychtoolbox mex/mx abstraction layer
        is not the source of trouble.  
	
	

*/


#include "WaitSecsMex.h"

void PsychGetPrecisionTimerSeconds(double *secs)
{
    UInt64	ticks;
    
    ticks=AudioGetCurrentHostTime();
    *secs=(double)(AudioConvertHostTimeToNanos(ticks) / (double)1000000000.0);
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double startTime, endTime, currentTime;
    

    PsychGetPrecisionTimerSeconds(&startTime);
    
    if(nlhs > 0)
        mexErrMsgTxt("WaitSecsMex uses no output arguments.");
    if(nrhs > 1)
        mexErrMsgTxt("WaitSecsMex requires only one input argument.");
    if(nrhs < 1)
        mexErrMsgTxt("WaitSecsMex requires one input argument.");
    if(mxGetM(prhs[0]) * mxGetN(prhs[0]) != 1)
        mexErrMsgTxt("WaitSecs requires 1x1 double as input argument");
    endTime=mxGetPr(prhs[0])[0] + startTime;
    for(PsychGetPrecisionTimerSeconds(&currentTime);currentTime<endTime;PsychGetPrecisionTimerSeconds(&currentTime))
        ;
        
}


/*
PsychError WAITSECSWaitSecs(void) 
{
    double 	*waitPeriod, readTime, endTime;
    
    //grab the time right away
    PsychGetPrecisionTimerSeconds(&readTime);
    

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    //Allocate a return matrix and load it with the depth values.
    PsychAllocInDoubleArg(1,TRUE,&waitPeriod);
    endTime = *waitPeriod + readTime; 
    
    while(readTime < endTime)
        PsychGetPrecisionTimerSeconds(&readTime);
        
    return(PsychError_none);	
}
*/
	
