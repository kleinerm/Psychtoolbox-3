/*
  PsychToolbox2/Source/Common/PsychStructGlue.h		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS: All
  
  PROJECTS:
  12/31/02	awi		Screen on OS X
   

  HISTORY:
  12/31/02  awi		wrote it.  
  
  DESCRIPTION:
  
	PsychStructGlue defines abstracted functions to create structs passed 
	between the calling environment and the PsychToolbox. 
  
  NOTES:
  
  
  
*/



//begin include once 
#ifndef PSYCH_IS_INCLUDED_StructGlue
#define PSYCH_IS_INCLUDED_StructGlue

#include "Psych.h"



psych_bool PsychAllocOutStructArray(	int position, 
                                        PsychArgRequirementType isRequired, 
                                        int numElements,
                                        int numFields, 
                                        const char **fieldNames,  
                                        PsychGenericScriptType **pStruct);
                                        

void PsychSetStructArrayStringElement(	char *fieldName,
                                        int index,
                                        char *text,
                                        PsychGenericScriptType *pStruct);
                                  
void PsychSetStructArrayDoubleElement(	char *fieldName,
                                        int index,
                                        double value,
                                        PsychGenericScriptType *pStruct);
                                        
void PsychSetStructArrayBooleanElement(	char *fieldName,
                                        int index,
                                        psych_bool state,
                                        PsychGenericScriptType *pStruct);
                                        
//You can use PsychSetStructArrayNativeElement instead of PsychSetStructArrayStructElement, the latter checks that pStructInner is really a struct.   
void PsychSetStructArrayStructElement(	char *fieldName,	
                                        int index,
                                        PsychGenericScriptType *pStructInner,
                                        PsychGenericScriptType *pStructOuter);
                                        
void PsychSetStructArrayNativeElement(	char *fieldName,
                                        int index,
                                        PsychGenericScriptType *nativeElement,
                                        PsychGenericScriptType *pStructOuter);
                                        
psych_bool PsychAssignOutStructArray(	int position, 
                                        PsychArgRequirementType isRequired,   
                                        PsychGenericScriptType *pStruct);

                                        
// see PsychScriptingGlue.h  
// void PsychAllocateNativeDoubleMat(int m, int n, int p, double **cArray, PsychGenericScriptType **nativeElement);

                                        

//use                                         

//end include once
#endif


