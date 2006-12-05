/*
  PsychToolbox3/Source/Common/TestProirityMex/TestPriorityMex.c		
  
  PROJECTS: TestPriority only.
  
  PLATFORMS:  Only OS X so far.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  4/21/03  awi		Created.
  9/22/03  awi		Rewrote to use same arguments as TimeTest standalone application and replaced ScriptingGlue calls with 
                        mx/mex API calls.
  
  DESCRIPTION:
  
  The function accepts the following arguments:
  
    POSITION	DESCRIPTION			UNITS		SIZE
        1	policy period			ticks		1x1
        2	policy computation		ticks		1x1
        3	policy constraint		ticks		1x1
        4	policy preemptible		flag		1x1
        5	blocking period 		seconds		1x1
        6	test  duration			seconds		1x1	
    

*/
// includes
#include "TestPriorityMex.h"

//name  the positions of values in the list of input  arguments.
#define POLICY_PERIOD_ARG		0
#define POLICY_COMPUTATION_ARG		1
#define POLICY_CONSTRAINT_ARG		2
#define POLICY_PREEMPTIBLE_ARG		3
#define LOOP_BLOCKINGPERIOD_ARG		4
#define LOOP_DURATION_ARG		5

#define PRETRIAL_DURATION_SECS		5


void PsychGetPrecisionTimerSeconds(double *secs)
{
    UInt64	ticks;
    
    ticks=AudioGetCurrentHostTime();
    *secs=(double)(AudioConvertHostTimeToNanos(ticks) / (double)1000000000.0);
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double					startTime, endTime, now, waitPeriodDouble;
    int 					estimatedNumLoops, numTimeValues;
    unsigned int				waitPeriodInt, i, memFactor=2;
    double					*timesBuffer, preTrialDurationRatio;
    thread_time_constraint_policy_data_t	tcPolicy;
    thread_standard_policy_data_t		sPolicy;
    task_t					threadID;
    int						kernError;
    bool					outOfMemoryFlag;
    	 
    //check for valid arguments and copy them in
    if(nrhs != 6)
        mexErrMsgTxt("Wrong number of arguments supplied to function.  TestPriorityMex uses exactly six arguments.");
    for(i=0;i<6;i++){
        if(mxGetM(prhs[i]) * mxGetN(prhs[i]) != 1 || !mxIsDouble(prhs[i])){
            mexPrintf("Argument %d must be 1x1 double\n", i);
            mexErrMsgTxt("Exiting");
        }
    }


    
    //Set time constraint  priority and  other test parameters from arguments.  
    //We don't need to save the old priority setting, we just set it to something reasonable when we are done with the test.
    if(mxGetPr(prhs[LOOP_BLOCKINGPERIOD_ARG])[0] > 1)
        mexErrMsgTxt("waitPeriod value must not exceed 1 second"); 
    waitPeriodDouble=mxGetPr(prhs[LOOP_BLOCKINGPERIOD_ARG])[0] * 1000000;  //IOSleep accepts time in unit of ms
    waitPeriodInt = (unsigned int)waitPeriodDouble;
    tcPolicy.period=(uint32_t)mxGetPr(prhs[POLICY_PERIOD_ARG])[0];
    tcPolicy.computation=(uint32_t)mxGetPr(prhs[POLICY_COMPUTATION_ARG])[0];
    tcPolicy.constraint=(uint32_t)mxGetPr(prhs[POLICY_CONSTRAINT_ARG])[0];
    tcPolicy.preemptible=(boolean_t)(mxGetPr(prhs[POLICY_PREEMPTIBLE_ARG])[0] ? TRUE : FALSE);
    threadID= mach_thread_self();
    kernError=thread_policy_set(threadID, THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&tcPolicy, THREAD_TIME_CONSTRAINT_POLICY_COUNT);

    //Run a trail loop of the specified duration to estimate how much memory we need to allocate
    //for storing the timing results
    mexPrintf("Executing pre-trial for specified test period of %d seconds....\n", PRETRIAL_DURATION_SECS);
    i=0;
    PsychGetPrecisionTimerSeconds(&startTime);
    endTime=startTime + PRETRIAL_DURATION_SECS;
    for(PsychGetPrecisionTimerSeconds(&now); now<endTime; PsychGetPrecisionTimerSeconds(&now)){ 
        usleep(waitPeriodInt);
        ++i; 
    }
    mexPrintf("Pre-trial loop complete.  Executed %d loops.\n", i);
    preTrialDurationRatio = mxGetPr(prhs[LOOP_DURATION_ARG])[0] / (double)PRETRIAL_DURATION_SECS;
    estimatedNumLoops=(int)(i * memFactor * preTrialDurationRatio);
    mexPrintf("Pre-trial loop complete.  Executed %d loops.\n", i);
    timesBuffer=(double *)malloc(estimatedNumLoops * sizeof(double));



    //Do the test and record the times.  This loop code is (trivially) different the standalone
    // timing test application. This is a bit better becasue we only read the time once through the loop.
    mexPrintf("Executing test for specified test period of %f seconds....\n", mxGetPr(prhs[LOOP_DURATION_ARG])[0]);
    i=0;
    PsychGetPrecisionTimerSeconds(&startTime);
    endTime=startTime+mxGetPr(prhs[LOOP_DURATION_ARG])[0];
    outOfMemoryFlag=FALSE;
    for(PsychGetPrecisionTimerSeconds(&(timesBuffer[i])); timesBuffer[i] < endTime; PsychGetPrecisionTimerSeconds(&(timesBuffer[i]))){
        usleep(waitPeriodInt);
        ++i;
        if(i == estimatedNumLoops){
            mexPrintf("Did not allocate sufficient memory to hold timing results:\n");
            mexPrintf("\tallocated:%d\n",estimatedNumLoops);
            mexPrintf("\tused:%d\n", i);
            mexPrintf("\ttestDuration:%f\n", mxGetPr(prhs[LOOP_DURATION_ARG])[0]);
            mexPrintf("\tpre-trial duration:%f\n", timesBuffer[i-1]);
            mexPrintf("\tpre-trial loops:%d\n", estimatedNumLoops);
            outOfMemoryFlag=TRUE;
            break;
	} 
    }
    numTimeValues=i-1;
              
    //restore standard policy.  Content of sPolicy is empty and unread by thread_policy_set().  
    kernError=thread_policy_set(threadID, THREAD_STANDARD_POLICY, (thread_policy_t)&sPolicy, THREAD_STANDARD_POLICY_COUNT);
    
    if(outOfMemoryFlag)
        mexErrMsgTxt("Exiting");
    else{
        //Create return matrix, copy out the result, and free the temporary buffer.
	plhs[0]=mxCreateDoubleMatrix(1, numTimeValues, mxREAL);
        for(i=0;i<numTimeValues;i++)
            mxGetPr(plhs[0])[i]=timesBuffer[i];
    }

    //free memory allocated  for storage during the timing loop
     free((void *)timesBuffer);

}




	
