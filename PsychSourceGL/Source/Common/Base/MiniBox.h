/* 
  MiniBox.h

  
  AUTHORS:
  Allen Ingling		awi		Allen.W.Ingling@nyu.edu  

  
  PLATFORMS: 
  11/25/01	awi		MacOS 9
  11/25/01	awi		Windows
  

  HISTORY
  11/25/01	awi		Created file.
  07/16/02  awi     Included PsychConstants.h and added include once.  
  

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_MiniBox
#define PSYCH_IS_INCLUDED_MiniBox


#include "Psych.h"

//define mex-derived data types.
//typedef	const mxArray	CONSTmxArray;
#if PSYCH_LANGUAGE == PSYCH_MATLAB
typedef void (*MexFunctionPtr)(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[]);
#endif

extern int PrintfExit(const char *format,...);
char *BreakLines(char *string,long lineLength);
Boolean PsychMatch(char *s1,char *s2);
char *int2str(int num);
int PsychIndexElementFrom2DArray(int mDim/*|Y|*/, int nDim/*|X|*/, int m/*y*/, int n/*x*/);
int PsychIndexElementFrom3DArray(int mDim/*|Y|*/, int nDim/*|X|*/, int pDim/*|Z|*/, int m/*y*/, int n/*x*/, int p/*z*/);
int PsychIndexPlaneFrom3DArray(int mDim, int nDim, int pDim, int planeIndex);
int maxInt(int a, int b);  //might be better to define max as a macro...
boolean PsychIsIntegerInDouble(double *value);

Boolean PsychIsPsychMatchCaseSensitive(void);
void PsychSetPsychMatchCaseSenstive(Boolean arg);

//end include once
#endif

