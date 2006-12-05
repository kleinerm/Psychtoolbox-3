/*
	TimeLists.c	

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS: 
	
		Only OS X for now.


	HISTORY:
	
		1/18/05		awi		Wrote it. 

	DESCRIPTION:

		For purposes of instrumenting Screen, maintain times samples in an abstract list type.  Internally we use a 
		linked list.  To external functions reading out values, the list appears to be an array.
		
		It is easy to time  Screen subfuntions from MATLAB by surrounding them with calls to GetSecs().  

		It is easy to time OpenGL and other C calls within  Screen subfunctions by surrionding them with 
		PsychGetAdjustedPrecisionTimerSeconds().  However, in this case there is no good way to get the time values back out of 
		Screen onto MATLAB for analysis.  TimeLists are part of the solution. Indivdual Screen subfunctions may use 
		a time list to store time samples.  Those time samples may then be read back into MATLAB using Screen('GetTimeList');
		To switch a Screen subfunction into "diagnostic" mode, where it fills a time array, you should create a Screen Preference
		setting accessible from MATLAB.
		
		Another way to do the same thing would be to allocate memory within MATLAB and have Screen store values directly
		in a MATLAB array.  But that has unpleasant drawbacks.  Either 1. We let MATLAB build up the list of times by 
		appending new values  to the end.  This is bad because MATLAB reallocates memory for the entire list every time it extends
		it by one element. 2. We precallocate the list in MATLAB.  This is bad because we have to know how long the list needs
		to be before we start recording into it.      
		
		The close routine which you register with ScriptingGlue, that routine which is executed before the mex file 
		is flushed, must call ClearTimingArray() to free storage allocated by TimeLists.
	
	TO DO:
	
		No provision is made for multiple simultaneous lists of times.  However, it seems better to only maintain one list of 
		times, and to have a second, parallel list of strings labeling the events which occured at particular times. 
		
*/

#include "Psych.h"



typedef struct _timeArrayElement_{
	double							timeValue;
	struct _timeArrayElement_		*next;
} timeArrayElement;


static timeArrayElement		*timeListHead=NULL;
static timeArrayElement		*timeListTail=NULL;
static unsigned int			numElements=0;


void StoreNowTime(void)
{
	double				now;
	timeArrayElement	*newElement;
	
	PsychGetAdjustedPrecisionTimerSeconds(&now);
	newElement=(timeArrayElement *)malloc(sizeof(timeArrayElement));
	newElement->timeValue=now;
	newElement->next=NULL;
	if(timeListTail != NULL)
		timeListTail->next=newElement;
	timeListTail=newElement;
	if(timeListHead==NULL)
		timeListHead=newElement;
	++numElements;
}

void ClearTimingArray(void)
{
	timeArrayElement	*thisElement, *tempElement;
	
	
	thisElement=timeListHead;
	while(thisElement!=NULL){
		if(thisElement->next==NULL){
			//end of list.  free this element.
			free(thisElement);
			thisElement=NULL;
			timeListHead=NULL;
			timeListTail=NULL;
			numElements=0;
		}else{
			//step ahead and free the element behind
			tempElement=thisElement;
			thisElement=thisElement->next;
			free(tempElement);
		}
	}	
}

unsigned int GetNumTimeValues(void)
{
	return(numElements);
}

unsigned int GetTimeArraySizeBytes(void)
{
	return(numElements * sizeof(double));
}


void CopyTimeArray(double *destination, unsigned int numElements)
{
	timeArrayElement	*thisElement;
	unsigned int		i;

	thisElement=timeListHead;
	for(i=0;i<numElements;i++){
		if(thisElement==NULL)
			PsychErrorExitMsg(PsychError_internal, "Attempted to copy out more values than are stored in list");
		else{
			destination[i]=thisElement->timeValue;
			thisElement=thisElement->next;
		}
	}
}














