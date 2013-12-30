/*
	PsychToolbox3/Source/Common/MachPriorityMex/MachPriorityMex.c
	
	PLATFORMS:	
	
		OS X only.  
			
	
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu
		Mario Kleiner		mk

	HISTORY:
	
		8/08/02			awi		wrote it.
		10/29/04		awi		fixed the spelling of "MachGetPriorityMex" in help lines.
		2/17/05         mk      Added proper error handling/reporting.
		4/6/05			awi		Merged Mario Kleiner's error handling improvements into Psychtoolbox.org master.  
        
	DESCRIPTION 
        
        MachSetPriorityMex is a simplified variant of MachPriority which strips out the Psychtoolbox mex/mx abstraction layer and separates 
        Get from Set.  See also MachGetPriorityMex
        
        Legal calls to MachSetPriorityMex:  
        
        priority=MachSetPriorityMex(['THREAD_STANDARD_POLICY'] );
        priority=MachSetPriorityMex(['THREAD_STANDARD_POLICY'], 'default');
        priority=MachSetPriorityMex(['THREAD_TIME_CONSTRAINT_POLICY', 'period','computation', 'constraint', 'preemptible');
        priority=MachSetPriorityMex(['THREAD_TIME_CONSTRAINT_POLICY', 'default');
        priority=MachSetPriorityMex(['THREAD_PRECEDENCE_POLICY', 'importance');
        priority=MachSetPriorityMex(['THREAD_PRECEDENCE_POLICY', 'default');
        
        
	NOTES:
        
        See MachGetPrioritMex.c notes section.
		
	IMPORTANT:  
		
		We seem to be missing some free statements below.  
        
*/

#include "MachPriorityMex.h"


	

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    bool						isError;
    thread_policy_flavor_t		flavorConstant;
    int							kernError;
    task_t						threadID;
    thread_policy_t				threadPolicy;
    mach_msg_type_number_t		policyCount, policyCountFilled;
    boolean_t					isDefault;
    char						commandString[COMMAND_STRING_LENGTH];
    
    
    threadID= mach_thread_self();
    kernError=0;
    
    //get the policy flavor constant specified by the user and the getDefault argument
    if(nrhs<1)
        mexErrMsgTxt("MachGetPriorityMex requires at least one argument.  See help MachGetPriorityMex.");
    if(!mxIsChar(prhs[0]))
        mexErrMsgTxt("First input argument is not a string.  See help MachGetPriorityMex.");
    mxGetString(prhs[0], commandString, COMMAND_STRING_LENGTH);
    isError=GetFlavorConstantFromFlavorString(commandString, mxGetM(prhs[0]) * mxGetN(prhs[0]), &flavorConstant);  //case sensitive.  
    if(isError)
        mexErrMsgTxt("Unrecognized command.  See help MachGetPriorityMex.");


    //branch according to the first argument
    switch(flavorConstant){
        case THREAD_STANDARD_POLICY:
            if(nrhs>2)
                mexErrMsgTxt("Extra argument(s) detected.  See help MachGetPriorityMex.");
            if(nrhs==2){
                if(!mxIsChar(prhs[1]))
                    mexErrMsgTxt("Expecting string in second argument.  See help MachGetPriorityMex.");
                mxGetString(prhs[1], commandString, COMMAND_STRING_LENGTH);
                commandString[COMMAND_STRING_LENGTH-1]= '\0';  //guarantee strcmp an end of string character
                if(strcmp(commandString, "default"))
                    mexErrMsgTxt("Unrecognized second argument.  See help MachGetPriorityMex.");
            }
            threadPolicy=(thread_policy_t)malloc(sizeof(thread_standard_policy_data_t));
            policyCount=THREAD_STANDARD_POLICY_COUNT;
            policyCountFilled=policyCount;
            isDefault=TRUE;
            kernError=thread_policy_get(threadID, THREAD_STANDARD_POLICY, threadPolicy, &policyCountFilled, &isDefault);
            if (kernError==0) kernError=thread_policy_set(threadID, THREAD_STANDARD_POLICY, threadPolicy, policyCountFilled);
            break; 
        case THREAD_TIME_CONSTRAINT_POLICY:
            if(nrhs==1)
                mexErrMsgTxt("Missing argument detected.  See help MachGetPriorityMex.");
            else if(nrhs==2){
                if(!mxIsChar(prhs[1]))
                    mexErrMsgTxt("Expecting string in second argument.  See help MachGetPriorityMex.");
                mxGetString(prhs[1], commandString, COMMAND_STRING_LENGTH);
                commandString[COMMAND_STRING_LENGTH-1]= '\0';  //guarantee strcmp an end of string character
                if(strcmp(commandString, "default"))
                    mexErrMsgTxt("Unrecognized second argument.  See help MachGetPriorityMex.");
                threadPolicy=(thread_policy_t)malloc(sizeof(thread_time_constraint_policy_data_t));
                policyCount=THREAD_TIME_CONSTRAINT_POLICY_COUNT;
                policyCountFilled=policyCount;
                isDefault=TRUE;
                kernError=thread_policy_get(threadID, THREAD_TIME_CONSTRAINT_POLICY, threadPolicy, &policyCountFilled, &isDefault);
                if (kernError==0) kernError=thread_policy_set(threadID, THREAD_TIME_CONSTRAINT_POLICY, threadPolicy, policyCountFilled);
                free((void*)threadPolicy);        
                break;
            }else if(nrhs != 5)
                mexErrMsgTxt("Incorrect number of arguments.  See help MachGetPriorityMex.");
            else{
                if(! (mxIsDouble(prhs[1]) && mxGetM(prhs[1]) * mxGetN(prhs[1]) == 1)) 
                    mexErrMsgTxt("Expected double in second argument.  See help MachSetPriorityMex.");
                if(! (mxIsDouble(prhs[2]) && mxGetM(prhs[2]) * mxGetN(prhs[2]) == 1)) 
                    mexErrMsgTxt("Expected double in third argument.  See help MachSetPriorityMex.");
                if(! (mxIsDouble(prhs[3]) && mxGetM(prhs[3]) * mxGetN(prhs[3]) == 1)) 
                    mexErrMsgTxt("Expected double in fourth argument.  See help MachGetPriorityMex.");
                if(!((mxIsDouble(prhs[4]) || mxIsLogical(prhs[4])) && (mxGetM(prhs[4]) * mxGetN(prhs[4]) == 1)))
                    mexErrMsgTxt("Expected double or logical in fifth argument.  See help MachSetPriorityMex.");
                threadPolicy=(thread_policy_t)malloc(sizeof(thread_time_constraint_policy_data_t));
                ((thread_time_constraint_policy_t)threadPolicy)->period=(uint32_t)mxGetPr(prhs[1])[0];
                ((thread_time_constraint_policy_t)threadPolicy)->computation=(uint32_t)mxGetPr(prhs[2])[0];
                ((thread_time_constraint_policy_t)threadPolicy)->constraint=(uint32_t)mxGetPr(prhs[3])[0];
                ((thread_time_constraint_policy_t)threadPolicy)->preemptible= (boolean_t)(mxIsDouble(prhs[4]) ? mxGetPr(prhs[4])[0] : mxGetLogicals(prhs[4])[0]);
                policyCount=THREAD_TIME_CONSTRAINT_POLICY_COUNT;
                policyCountFilled=policyCount;
                kernError=thread_policy_set(threadID, THREAD_TIME_CONSTRAINT_POLICY, threadPolicy, policyCountFilled);
                free((void*)threadPolicy);
                break;
            }
        case THREAD_PRECEDENCE_POLICY:
            if(nrhs>2)
                mexErrMsgTxt("Extra argument(s) detected.  See help MachGetPriorityMex.");
            if(nrhs<2)
                mexErrMsgTxt("Missing argument detected.  See help MachGetPriorityMex.");
            if(mxIsChar(prhs[1])){  //set the default
                mxGetString(prhs[1], commandString, COMMAND_STRING_LENGTH);
                commandString[COMMAND_STRING_LENGTH-1]= '\0';  //guarantee strcmp an end of string character
                if(strcmp(commandString, "default"))
                    mexErrMsgTxt("Unrecognized second argument.  See help MachGetPriorityMex.");
                threadPolicy=(thread_policy_t)malloc(sizeof(thread_precedence_policy_data_t));
                policyCount=THREAD_PRECEDENCE_POLICY_COUNT;
                policyCountFilled=policyCount;
                isDefault=TRUE;
                kernError=thread_policy_get(threadID, THREAD_PRECEDENCE_POLICY, threadPolicy, &policyCountFilled, &isDefault);
                if (kernError==0) kernError=thread_policy_set(threadID, THREAD_PRECEDENCE_POLICY, threadPolicy, policyCountFilled);
                free((void*)threadPolicy);
                break; 
            }else if(mxIsDouble(prhs[1]) && mxGetM(prhs[1]) * mxGetN(prhs[1]) == 1){  //set a specified value
                threadPolicy=(thread_policy_t)malloc(sizeof(thread_precedence_policy_data_t));
                ((thread_precedence_policy_t)threadPolicy)->importance=(integer_t)mxGetPr(prhs[1])[0];
                policyCount=THREAD_PRECEDENCE_POLICY_COUNT;
                policyCountFilled=policyCount;
                kernError=thread_policy_set(threadID, THREAD_PRECEDENCE_POLICY, threadPolicy, policyCountFilled);
            }
    }

    // Check for and report errors in thread_policy_set:
    if (kernError!=0) {
        mexErrMsgTxt("ERROR: Failed to set requested thread scheduling policy! thread_policy_set() failed!");
    }
}
