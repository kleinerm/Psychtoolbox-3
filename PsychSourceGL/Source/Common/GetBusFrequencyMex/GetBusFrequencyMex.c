/*
	PsychToolbox3/Source/Common/GetBusFrequencyMex/GetBusFrequencyMex.c
	
	PLATFORMS:	Matlab on OSX only
			

	AUTHORS:
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
		8/12/03			awi		wrote it. 
		
	DESCRIPTION 
		
		GetBusFrequencyMex returns the system bus frequency. Values read and written by MachGetPriorityMex and
		MachSetPriorityMex when specifying THREAD_TIME_CONSTRAINT_POLICY (realtime) paramters are in units of bus
		ticks.  Use the tick rate as returned by GetBusFrequencyMex to convert units into seconds. 
		
	TARGET LOCATION:
		SleepSecs.mexmac resides in:
			PsychToolbox/PsychObsolete/
		
	To change the target location modify the script:
		Psychtoolbox/Tools/Scripts/CopyOutMexFiles/GetBusFrequency_CopyOut.sh


*/


#include "GetBusFrequencyMex.h"


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    int		 mib[2], tempInt;
    size_t	tempIntSize;	
    
    if(nlhs > 1)
        mexErrMsgTxt("GetBusFrequencyMex uses no more than one return argument.");
    if(nrhs > 0)
        mexErrMsgTxt("GetBusFrequencyMex uses no input arguments.");
     
    mib[0]=CTL_HW;
    mib[1]=HW_BUS_FREQ;
    tempIntSize=sizeof(tempInt);
    sysctl(mib, 2, &tempInt, &tempIntSize, NULL, 0);
   
        
    plhs[0]=mxCreateDoubleMatrix(1, 1, mxREAL);
    mxGetPr(plhs[0])[0]=(double)tempInt;   
    
}





