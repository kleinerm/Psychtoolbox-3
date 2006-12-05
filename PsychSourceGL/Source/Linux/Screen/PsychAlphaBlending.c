/*
PsychAlphaBlending.c

PLATFORMS:	

	Only OS X for now.  
			
AUTHORS:
	
	Allen Ingling		awi		Allen.Ingling@nyu.edu

HISTORY:
	
	1/7/04		awi		Wrote it 
						
DESCRIPTION:

	Functions for getting and setting the alpha blending rule in windows.  To assure consistent treatement of state ONLY functions in this file should
	be used as a gateway to modify Screen state.
	
*/
	

#include "Screen.h"


#define DEFAULT_SRC_ALPHA_FACTOR		GL_SRC_ALPHA
#define DEFAULT_DST_ALPHA_FACTOR		GL_ONE

#define NUM_BLENDING_MODE_CONSTANTS		11

char *blendingModeStrings[]={
	"GL_ZERO",
	"GL_ONE",
	"GL_SRC_COLOR",
	"GL_ONE_MINUS_SRC_COLOR",
	"GL_DST_COLOR",
	"GL_ONE_MINUS_DST_COLOR",
	"GL_SRC_ALPHA",
	"GL_ONE_MINUS_SRC_ALPHA",
	"GL_DST_ALPHA",
	"GL_ONE_MINUS_DST_ALPHA",
	"GL_SRC_ALPHA_SATURATE"
};

GLenum blendingModeConstants[]={
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA_SATURATE 
};


/* 
	PsychValidateBlendingConstantForSource()
	
	Constants are specified for both the source and destination.  Not all constants are available for both.  This function checks
	a constnat to see if it may be specified for the source.
	
	Return TRUE if the choice if valid and false otherwise.
	
*/
Boolean PsychValidateBlendingConstantForSource(GLenum sourceFactor)
{
	Boolean isValid;

	switch(sourceFactor)
	{
		case GL_ZERO:					isValid=TRUE;  break;
		case GL_ONE:					isValid=TRUE;  break;	
		case GL_SRC_COLOR:				isValid=FALSE;  break;
		case GL_ONE_MINUS_SRC_COLOR:	isValid=FALSE;  break;
		case GL_DST_COLOR:				isValid=TRUE;  break;
		case GL_ONE_MINUS_DST_COLOR:	isValid=TRUE;  break;
		case GL_SRC_ALPHA:				isValid=TRUE;  break;
		case GL_ONE_MINUS_SRC_ALPHA:	isValid=TRUE;  break;
		case GL_DST_ALPHA:				isValid=TRUE;  break;
		case GL_ONE_MINUS_DST_ALPHA:	isValid=TRUE;  break;
		case GL_SRC_ALPHA_SATURATE:		isValid=TRUE;  break; 
		default: PsychErrorExitMsg(PsychError_internal, "Failed to find alpha blending factor constant when validating for source specification");
	}
	return(isValid);
}


/* 
	PsychValidateBlendingConstantForSource()
	
	Constants are specified for both the source and destination.  Not all constants are available for both.  This function checks
	a constant to see if it may be specified for the destination.
	
	Return TRUE if the choice if valid and false otherwise.
  
*/
Boolean PsychValidateBlendingConstantForDestination(GLenum destinationFactor)
{
	Boolean isValid;
	
	switch(destinationFactor)
	{
		case GL_ZERO:					isValid=TRUE;  break;
		case GL_ONE:					isValid=TRUE;  break;	
		case GL_SRC_COLOR:				isValid=TRUE;  break;
		case GL_ONE_MINUS_SRC_COLOR:	isValid=TRUE;  break;
		case GL_DST_COLOR:				isValid=FALSE;  break;
		case GL_ONE_MINUS_DST_COLOR:	isValid=FALSE;  break;
		case GL_SRC_ALPHA:				isValid=TRUE;  break;
		case GL_ONE_MINUS_SRC_ALPHA:	isValid=TRUE;  break;
		case GL_DST_ALPHA:				isValid=TRUE;  break;
		case GL_ONE_MINUS_DST_ALPHA:	isValid=TRUE;  break;
		case GL_SRC_ALPHA_SATURATE:		isValid=FALSE;  break; 
		default: PsychErrorExitMsg(PsychError_internal, "Failed to find alpha blending factor constant when validating for destination specification");
	}
	return(isValid);
}




/*  
	PsychGetGLBlendConstantFromString()
	
	Lookup the constant from the string.  Return true if we find it.
*/
int PsychGetAlphaBlendingFactorConstantFromString(char *blendString, GLenum *blendConstant)
{
	int		i;
	
	for(i=0;i<NUM_BLENDING_MODE_CONSTANTS;i++){
		if(PsychMatch(blendingModeStrings[i], blendString)){
			*blendConstant=blendingModeConstants[i];
			return(TRUE);
		}
	}
	return(FALSE);
}


/*
	stringSize	PsychGetGLBlendStringFromConstant(GLenum blendConstant, NULL);
	foundIt		PsychGetGLBlendSTringFromConstnat(GLenum blendConstant, char *blendString)

	Lookup the string from the constant.  The caller allocates the return string.
	to find the length of the return string, first call lookup with NULL for the 
	return.  Call a second time and pass the pointer to your allocated space.  
	
	PsychGetBlendStringFromConstant will always return 0 if it fails to find the 
	constant.
*/
int PsychGetAlphaBlendingFactorStringFromConstant(GLenum blendConstant, char *blendString)
{
	int		i;
	
	for(i=0;i<NUM_BLENDING_MODE_CONSTANTS;i++){
		if(blendConstant==blendingModeConstants[i]){
			if(blendString!=NULL)
				strcpy(blendString, blendingModeStrings[i]);
			return(strlen(blendingModeStrings[i]) + 1);
		}
	}
	return(0);
}



/*
	PsychInitWindowRecordAlphaBlendingFields()
	
	Called when we create a new window.
*/
void PsychInitWindowRecordAlphaBlendingFactors(PsychWindowRecordType *winRec)
{
	winRec->actualEnableBlending=FALSE;
	winRec->actualSourceAlphaBlendingFactor=GL_ONE;				
	winRec->actualDestinationAlphaBlendingFactor=GL_ZERO;
	winRec->nextSourceAlphaBlendingFactor=GL_ONE;
	winRec->nextDestinationAlphaBlendingFactor=GL_ZERO;
	
}

/*
	PsychGetBlendConstantsFromWindow()
	
	Get the blending constants which will be used if we draw into the window.
*/
void PsychGetAlphaBlendingFactorsFromWindow(PsychWindowRecordType *winRec, GLenum *oldSource, GLenum *oldDestination)
{
	*oldSource=winRec->nextSourceAlphaBlendingFactor;
	*oldDestination=winRec->nextDestinationAlphaBlendingFactor;
}


/* 
	PsychStoreAlphaBlendingFactorsForWindow()
	
	To avoid unnecessary context switching, only store the desired blending factors in the window record.  Defer calls to glBlendFunc()
	until a drawing function, which must change context, invokes PsychUpdateAlphaBlendingFactorLazily() which in turn calls glBlendFunc()
	the blending mode has changed.  
*/
void PsychStoreAlphaBlendingFactorsForWindow(PsychWindowRecordType *winRec, GLenum sourceBlendConstant, GLenum destinationBlendConstant)
{
	winRec->nextSourceAlphaBlendingFactor=sourceBlendConstant;
	winRec->nextDestinationAlphaBlendingFactor=destinationBlendConstant;
}


/*
	PsychUpdateAlphaBlendingFactorLazily()
*/
void PsychUpdateAlphaBlendingFactorLazily(PsychWindowRecordType *winRec)
{
	// Defer invokation of glBlendFunc() in either of two circumstances:
	// 1.  To prevent unnecessary context switching, When calling Screen('BlendFunction') we do not invoke glBlendFunc(). Rather,   
	//      'BlendFunction' stores the intended blending mode in the window record.  Any Screen subfunction which draws into the
	//		window should invokes PsychUpdateAlphaBlendingFactorLazily() to lookup the stored mode in the window record and set that.
	// 2.  Before calling glBlendFunc(), PsychUpdateAlphaBlendingFactorLazily()  compares the current blend settings to those in the window
	//     record.  If they match, then we skip the unnecessary call to glBlendFunc().
	
	// glDisable(GL_BLEND) is the same thing as the combination of glEnable(GL_BLEND) and glBlendFunc(GL_ONE, GL_ZERO).  
	// Therefore, when the user selects GL_ONE, GL_ZERO  we have an arbitrary choice to make between those alternatives.
	// GL contexts are born with blending disabled.  We choose the alternative which results in the fewest GL calls:
	// GL_BLEND disabled until the first change from GL_ONE, GL_ZERO, and thereafter leave GL_BLEND enabled.   
	
	Boolean		changeSetting, setEnable;
	
	changeSetting= winRec->actualSourceAlphaBlendingFactor != winRec->nextSourceAlphaBlendingFactor || winRec->actualDestinationAlphaBlendingFactor != winRec->nextDestinationAlphaBlendingFactor;
	setEnable= (winRec->nextSourceAlphaBlendingFactor != GL_ONE || winRec->nextDestinationAlphaBlendingFactor != GL_ZERO) && !winRec->actualEnableBlending;
	if(setEnable){
		winRec->actualEnableBlending=TRUE;	
		glEnable(GL_BLEND);
	}
	if(changeSetting){
		winRec->actualSourceAlphaBlendingFactor=winRec->nextSourceAlphaBlendingFactor;
		winRec->actualDestinationAlphaBlendingFactor = winRec->nextDestinationAlphaBlendingFactor;
		glBlendFunc(winRec->actualSourceAlphaBlendingFactor, winRec->actualDestinationAlphaBlendingFactor);
	}
}



