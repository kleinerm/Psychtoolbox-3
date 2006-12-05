/*
	PsychToolbox3/Source/Common/MachPriorityMex/MachPriorityMex.c
	
	PLATFORMS:	OS X only.  
			
	
	AUTHORS:
	Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	8/08/02			awi		wrote it. 
        
        DESCRIPTION 
        
        MachPriorityMex is a simplified variant of MachPriority which strips out the Psychtoolbox mex/mx abstraction layer.
        For the sake of greater readability it uses a reduced argument set.
        
        Calling forms are eiether:
        MachPriorityMex('Restore');				Restore the previous priority settings existing before the first invocation of 
                                                                MachPrioritySet.
        
        MachPriorityMex('Default');				Restore the default priority settings for they system.
                                                       
        MachPriorityMex(period, computation, constraint);	Set real-time priority with the specified parameters.
*/
        
        
#include "MachPriorityMex.h"


/*
    GetFlavorStringFromFlavorConstant()
    
*/


/*
static thread_policy_flavor_t flavorConstants[] = {THREAD_STANDARD_POLICY, THREAD_EXTENDED_POLICY, THREAD_TIME_CONSTRAINT_POLICY, THREAD_PRECEDENCE_POLICY};
static char *flavorStrings[] = {"THREAD_STANDARD_POLICY", "THREAD_EXTENDED_POLICY","THREAD_TIME_CONSTRAINT_POLICY", "THREAD_PRECEDENCE_POLICY"};
static int numFlavors=4;
*/

static thread_policy_flavor_t flavorConstants[] = {THREAD_STANDARD_POLICY, THREAD_TIME_CONSTRAINT_POLICY, THREAD_PRECEDENCE_POLICY};
static char *flavorStrings[] = {"THREAD_STANDARD_POLICY", "THREAD_TIME_CONSTRAINT_POLICY", "THREAD_PRECEDENCE_POLICY"};
static int numFlavors=3;

/*
    GetFlavorStringFromFlavorConstant()
    
    returns NULL if no matching flavor string is found.  
*/

char *GetFlavorStringFromFlavorConstant(thread_policy_flavor_t flavor)
{
    int  i;
    
    for(i=0;i<=numFlavors;i++){
        if(flavorConstants[i]==flavor)
            return(flavorStrings[i]);
    }
    return(NULL);  
}


/*
    GetFlavorConstantFromFlavorString()
    
    The return value is and error signal; returns 0 if the flavor string is valid and 1 otherwise.
*/
bool GetFlavorConstantFromFlavorString(char *flavorString, int flavorStringLength, thread_policy_flavor_t *flavorConstant)  //case sensitive.  
{
    int i;

    for(i=0;i<numFlavors;i++){
        if(!strncmp(flavorStrings[i], flavorString, MIN(flavorStringLength, COMMAND_STRING_LENGTH))){
            *flavorConstant=flavorConstants[i];
            return(0);
        }
    }
    return(1);
}


