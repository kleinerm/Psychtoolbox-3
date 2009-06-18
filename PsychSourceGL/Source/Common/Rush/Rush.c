/*
	PsychToolbox3/Source/Common/Rush/Rush.c		

	PLATFORMS:

		Only OS X for now.

	AUTHORS:

		Allen.Ingling@nyu.edu		awi 

	HISTORY:

		12/11/03	awi		Derived from Denis Pelli's Rush for the OS 9.
		11/3/04		awi		Fixed warnings which resulted from missing const in type specification of local variable definition.  


	NOTES: 
  
		Some obsolte features of the OS 9 Rush which have been removed:
			¥ We do not need to disable backgroundig because no version of MATLAB for OS X has backgrounding.
			¥ We no longer support the debug mode. It was used "to distinguish Mac OS (and device driver) interrupts 
				"from Matlab delays."  That is not necessary because Rush on OS X does not operate by disabling
				device interrupts.
				
			
		MATLAB:
			mexCallMATLAB(...,"eval") works differently in OS X than in OS 9.  Rush has been altered accordingly.  mexCallMATLAB has the error that
			it will ignore the mexSetTrapFlag() flag if the catch argument to eval is not valid.  We now detect errors by examining the lasterr string
			within Rush.  Checking the error flag returned by mexCallMATLAB no longer seems to work.  
			
		 PsychScriptingGlue:
			We get lazy about abstracting up the call through MATLAB to the Priority script.  Eventually we can make calls straight into 
			the Darwin set priority function instead of through a MATLAB script, once we no longer have to call Priority to shut down update.  
			Rush is an oddball Psychtoolbox function which makes calls back into MATLAB, and PsychScriptingGlue does not handle that well.
			Attempts were made to adapt scripting glue, but it was ugly.  Usually its the other way around
			and ScriptingGlue is more elegent than the mex API.  Probably best to just use straight mex call here and not spend the time to devise 
			a good abstraction layer for that, since it would be rarely used.  Plus, binding a few more languages would give some sense about 
			what is the best way to   
  
    BUGS:
    
    TO DO:  
    
        ¥ We could use evalc and cache the printed output of the rushed code until we restore normal priority.  
        
        ¥ It would be nice if rushed functions returned arguments through Rush, for example "secs = Rush('GetSecs',1)".
        There seems to be no easy way to achieve this.
*/


#include "Rush.h"

#define DEFAULT_PRIORITY_LEVEL 			0
#define MAX_ERROR_STRING_LENGTH			512
#define RUSH_ERROR_ANNOUNCEMENT_PREFIX		"Error during Rush: "
#define	MAX_PRIORITY_READBACK_ERROR		(double)0.001


PsychError RUSHRush(void) 
{
    double							recalledSetPriorityLevel, priorityLevelSpecifiedDouble, originalPriority;
	const PsychGenericScriptType	*codeNativeString, *codeNativeCell;
    PsychGenericScriptType			*evalCallInputs[2], *priorityCallInputs_1[1], *priorityCallOutputs_1[1], *priorityCallOutputs_2[1];
    PsychGenericScriptType			*evalCallOutputs[1], *lasterrCallInputs[1], *lasterrCallOutputs[1]; 
    psych_bool							isCellArray, isCharArray, rushError;
    int								numPriorityCallInputs, numPriorityCallOutputs, numEvalCallInputs, numEvalCallOutputs, errorPrefixStringLength;
    int								numLasterrCallInputs, numLasterrCallOutputs, error, error2;
    char							errorString[MAX_ERROR_STRING_LENGTH];

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(2));
    
    //fetch the code to be rushed.  this can be either a string or a cell array of strings.  If its a cell array we need to convert it to a string.
    isCellArray = PsychAllocInNativeCellVector(1, kPsychArgAnything, &codeNativeCell);
    isCharArray = PsychAllocInNativeString(1, kPsychArgAnything, &codeNativeString);
    if(!(isCellArray || isCharArray)) PsychErrorExitMsg(PsychError_user, "First input argument must be either a cell array or a string.");
    
	if(isCellArray) PsychConvertNativeCellArrayToNativeString(codeNativeCell, &codeNativeString);
        
    //fetch the priority level and bounds check.    
    priorityLevelSpecifiedDouble=DEFAULT_PRIORITY_LEVEL;
    PsychCopyInDoubleArg(2, kPsychArgOptional, &priorityLevelSpecifiedDouble);
    if(priorityLevelSpecifiedDouble < 0 || priorityLevelSpecifiedDouble > 9)
        PsychErrorExitMsg(PsychError_user, "Priority level is out of bounds.");
        
    //set the priority through the Psychtoolbox priority script.
    //PsychAllocOutDoubleArg_2(kPsychNoArgReturn, TRUE, &priorityLevelDouble, priorityCallInputs_1[0]);
    //*priorityLevelDouble=priorityLevelSpecifiedDouble;
    priorityCallInputs_1[0]=mxCreateDoubleMatrix(1,1,mxREAL);
    mxGetPr(priorityCallInputs_1[0])[0]=priorityLevelSpecifiedDouble;
    priorityCallOutputs_1[0]=NULL;
    numPriorityCallOutputs=1;
    numPriorityCallInputs=1;
    error=mexCallMATLAB(numPriorityCallOutputs, priorityCallOutputs_1, numPriorityCallInputs, priorityCallInputs_1, "Priority");
    if(error)
        PsychErrorExitMsg(PsychError_user, "Failed to set the priority.");
    originalPriority=mxGetPr(priorityCallOutputs_1[0])[0];
    
    //Clear MATLAB's last error so that MATLAB will return an empty string until the next error is registered.
    // this should not be necessary (though it apppeared in the OS 9 version), but we could use it as another means of detecting whether the rushed
    // code gave an error (see next comment).  
    mexEvalString("lasterr('');");
    
    //Call eval on the rushed code. Use the "catch" feature of eval by supplying a second argument to eval.  Using "catch" makes MATLAB  store the error string
    // from evaluating the first argument in the "lasterr" holding place.  However, using catch would also cause the mexCallMATLAB function to not
    // signal an error if "catch" evaluated without error.  Therefore we should give a "catch" argument which does not evaluate correctly; we use ";"
    // in conjunction with a return argument.  (The assignment of no value to a return argument generates  an error.)
    numEvalCallOutputs=0;
    numEvalCallInputs=2;
    evalCallOutputs[0]=NULL;
    evalCallInputs[0]=(PsychGenericScriptType*)codeNativeString;  //discard const type qualifier.  
    evalCallInputs[1]=mxCreateString("1");  //should use PsychAllocOutChar with the kPsychNoArgReturn argument.
    mexSetTrapFlag(TRUE);
    error=mexCallMATLAB(numEvalCallOutputs,evalCallOutputs,numEvalCallInputs,evalCallInputs,"eval");
    mexSetTrapFlag(FALSE);


    //restore the priority through the Psychtoolbox priority script
    error=mexCallMATLAB(numPriorityCallOutputs,  priorityCallOutputs_2, numPriorityCallInputs, priorityCallOutputs_1, "Priority");
    recalledSetPriorityLevel=mxGetPr(priorityCallOutputs_2[0])[0];
    if( fmax(priorityLevelSpecifiedDouble,recalledSetPriorityLevel) - fmin(priorityLevelSpecifiedDouble,recalledSetPriorityLevel) >  MAX_PRIORITY_READBACK_ERROR)
        PsychErrorExitMsg(PsychError_internal, "Priority after Rush does not match specifed value"); 
        
   
    //Check to see if the rushed code produced an error by examining the error string returned by MATLAB's lasterr. 
    numLasterrCallOutputs=1;
    numLasterrCallInputs=0;
    lasterrCallOutputs[0]=NULL;
    lasterrCallInputs[0]=NULL;	//Placeholder.  It's not clear what is the minimum allowable.  
    error2=mexCallMATLAB(numLasterrCallOutputs,lasterrCallOutputs,numLasterrCallInputs,lasterrCallInputs,"lasterr");
    error2=mxGetString(lasterrCallOutputs[0], errorString, MAX_ERROR_STRING_LENGTH -1);
    rushError= strlen(errorString) != 0;
    
    
    //issue the error message if there was an error. A better way to check for this might be to read back the lasterr string.
    if(rushError){
        strcpy(errorString, RUSH_ERROR_ANNOUNCEMENT_PREFIX);
        errorPrefixStringLength=strlen(errorString);
        error2=mxGetString(lasterrCallOutputs[0], errorString + errorPrefixStringLength, MAX_ERROR_STRING_LENGTH - errorPrefixStringLength -1);
        mexErrMsgTxt(errorString);
    }

    return(PsychError_none);	
}




	
