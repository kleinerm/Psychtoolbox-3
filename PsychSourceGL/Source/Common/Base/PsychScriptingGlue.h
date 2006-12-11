/*
  PsychToolbox2/Source/Common/PsychScriptingGlue.h		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: This the Matlab version for all platforms.  
  
  PROJECTS:
  08/12/02	awi		Screen on MacOS9
   

  HISTORY:
  08/12/02  awi		wrote it.  
  
  DESCRIPTION:
  
	ScriptingGlue defines abstracted functions to pass values 
	between the calling environment and the PsychToolbox.  
   
  
  TO DO: 
	  
    The PsychGet* and PsychPut* functions should be modified to return booleans
*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_ScriptingGlue
#define PSYCH_IS_INCLUDED_ScriptingGlue

#include "Psych.h"


//#define OPTIONAL				FALSE
//#define REQUIRED				TRUE
#define kPsychUseDefaultArgPosition		-1   //see ScreenArguments.h for more argument position defaults.  We should get rid of this.  
#define kPsychNoArgReturn			-1

//typedefs
typedef double 	*PsychFlagListType;  

//NaN
double PsychGetNanValue(void);

// Simple function evaluation by scripting environment via feval() style functions:
int		PsychRuntimeEvaluateString(const char* cmdstring);

//for integers
boolean PsychCopyInIntegerArg(int position, PsychArgRequirementType isRequired, int *value);
boolean PsychAllocInIntegerListArg(int position, PsychArgRequirementType isRequired, int *numElements, int **array);

//for doubles
boolean PsychCopyInDoubleArg(int position, PsychArgRequirementType isRequired, double *value);
boolean PsychAllocInDoubleArg(int position, PsychArgRequirementType isRequired, double **value);
boolean PsychAllocInDoubleMatArg(int position, PsychArgRequirementType isRequired, int *m, int *n, int *p, double **array);
boolean PsychCopyOutDoubleArg(int position, PsychArgRequirementType isRequired, double value);
//boolean PsychAllocOutDoubleArg_2(int position, PsychArgRequirementType isRequired, double **value, ...);  //NEW 
boolean PsychAllocOutDoubleArg(int position, PsychArgRequirementType isRequired, double **value);
boolean PsychAllocOutDoubleMatArg(int position, PsychArgRequirementType isRequired, int m, int n, int p, double **array);
boolean PsychCopyOutDoubleMatArg(int position, PsychArgRequirementType isRequired, int m, int n, int p, double *fromArray);
    //PsychAllocateNativeDoubleMat is for use with cell arrays and structs.  The right way to do this is to use the normal function for returning 
    //doubles, detect if the position is -1, and if so accept the optional "nativeElement" value.   
void 	PsychAllocateNativeDoubleMat(int m, int n, int p, double **cArray, PsychGenericScriptType **nativeElement);	

//for boolean.  These should be consolidated with the flags below. 
boolean PsychAllocOutBooleanMatArg(int position, PsychArgRequirementType isRequired, int m, int n, int p, PsychNativeBooleanType **array);
boolean PsychCopyOutBooleanArg(int position, PsychArgRequirementType isRequired, PsychNativeBooleanType value);
boolean PsychAllocOutBooleanArg(int position, PsychArgRequirementType isRequired, PsychNativeBooleanType **value);

//for flags.  
boolean PsychCopyInFlagArg(int position, PsychArgRequirementType isRequired, boolean *argVal);
boolean PsychAllocInFlagArg(int position, PsychArgRequirementType isRequired, boolean **argVal);
boolean PsychAllocInFlagArgVector(int position,  PsychArgRequirementType isRequired, int *numElements, boolean **argVal);
boolean PsychCopyOutFlagArg(int position, PsychArgRequirementType isRequired, boolean argVal);
boolean PsychAllocOutFlagListArg(int position, PsychArgRequirementType isRequired, int numElements, PsychFlagListType *flagList); //flag lists are opaque.
void PsychLoadFlagListElement(int index, boolean value, PsychFlagListType flagList);  	  
void PsychSetFlagListElement(int index, PsychFlagListType flagList);
void PsychClearFlagListElement(int index, PsychFlagListType flagList);


//for bytes
boolean PsychAllocInUnsignedByteMatArg(int position, PsychArgRequirementType isRequired, int *m, int *n, int *p, unsigned char **array);
boolean PsychAllocOutUnsignedByteMatArg(int position, PsychArgRequirementType isRequired, int m, int n, int p, ubyte **array);

//for strings
boolean PsychAllocInCharArg(int position, PsychArgRequirementType isRequired, char **str);
boolean PsychCopyOutCharArg(int position, PsychArgRequirementType isRequired, const char *str);


//query and govern argumuments.  Use these sparingly, usually you can let the "PsychAlloc*" and "PsychCopy*" functions above will do the work for you.  
int PsychGetNumInputArgs(void);
int PsychGetNumOutputArgs(void);
PsychError PsychCapNumInputArgs(int maxInputs);
PsychError PsychRequireNumInputArgs(int minInputs);
PsychError PsychCapNumOutputArgs(int maxNamedOutputs);
int PsychGetNumNamedOutputArgs(void);
boolean PsychIsArgPresent(PsychArgDirectionType direction, int position);
boolean PsychIsArgReallyPresent(PsychArgDirectionType direction, int position);
PsychArgFormatType PsychGetArgType(int position); //this is for inputs because outputs are unspecified
int PsychGetArgM(int position);
int PsychGetArgN(int position);
int PsychGetArgP(int position);
void PsychErrMsgTxt(char *s);
void PsychEnableSubfunctions(void);
boolean PsychAreSubfunctionsEnabled(void);
boolean PsychCheckInputArgType(int position, PsychArgRequirementType isRequired, PsychArgFormatType argType);


//for the benefit of PsychStructGlue and PsychCellGlue.  Don't use these unless you are writing more glue libraries. 
//They should probably be moved to a separate header file.
PsychError PsychSetReceivedArgDescriptor(int argNum, PsychArgDirectionType direction);
PsychError PsychSetSpecifiedArgDescriptor(	int			position,
                                                        PsychArgDirectionType 	direction,
                                                        PsychArgFormatType 	type,
                                                        PsychArgRequirementType	isRequired,
                                                        int			mDimMin,		// minimum minimum is 1   |   
                                                        int			mDimMax, 		// minimum maximum is 1, maximum maximum is -1 meaning infinity
                                                        int			nDimMin,		// minimum minimum is 1   |   
                                                        int			nDimMax,		// minimum maximum is 1, maximum maximum is -1 meaning infinity
                                                        int 		pDimMin,	    // minimum minimum is 0
                                                        int			pDimMax);
Boolean PsychAcceptInputArgumentDecider(PsychArgRequirementType isRequired, PsychError matchError);
Boolean PsychAcceptOutputArgumentDecider(PsychArgRequirementType isRequired, PsychError matchError);	
PsychError PsychMatchDescriptors(void);
mxArray **PsychGetOutArgMxPtr(int position);
const mxArray *PsychGetInArgMxPtr(int position);

//end include once
#endif


