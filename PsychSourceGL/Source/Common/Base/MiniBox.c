/* 
MiniBox.cpp

DESCRIPTION:

	The minimal set of functions necessary to support SCREEN style error messages, help and 
	argument list parsing.  Originally copied from David Brainard and Denis Pelli's Psychtoolbox project,
	with some modification.  It has since evolved into a miscellaneous category.  

  
HISTORY: 

	08/22/01	awi		Windows
	11/15/01	awi		Mac OS 9  


	08/20/01	awi		created file.
	08/20/01	awi		replaced fancy memory allocation in PrintfExit with static array. 
	11/26/01	awi		replaced references to ActiveWireTable in PsychMatch with ProjectTable
						This is so we can share the MiniBox file between ActiveWire and 
						Joystick projects. 
	12/17/01	awi		Commented out include "joystick.h" and included stdafx.h for GetChar project.
					
	12/17/01	awi		All of the grief with ordering the header inclusion and porbability was 
						caused by PsychMatch using the project table to read the case-sensitive 
						character comparision flag from the psych table.  That's a pretty useless
						feature anyway, so I just commented that out to make GetChar compile.
	1/20/04		awi		Cosmetic.

TO DO:
  
	  PrintfExit uses mexErrMsgTxt(s), the function call should be abstracted up for other scripting
	  languages and moved ScriptingGlue.
	 

*/

#include "Psych.h"

#define MAX_PRINTFEXIT_LEN  2000 

static Boolean			isPsychMatchCaseSensitive=FALSE;


/* PrintfExit used some fancy stuff to allocate the memory
   which holds the the error message string passed to 
   mexErrMsgTxt.  I just used a static array here. -awi
*/
int PrintfExit(const char *format,...)
{
	char s[MAX_PRINTFEXIT_LEN];
	va_list args;
	int i;
		
	va_start(args,format);
	i=vsprintf(s,format,args);
	va_end(args);
	if(i+1>MAX_PRINTFEXIT_LEN)
		printf("buffer overrun in PrintfExit\n");
	PsychErrMsgTxt(s); //mexErrMsgTxt(s);

	return 0;
}



char *BreakLines(char *string,long lineLength)
{
	long i,leftMargin,rightMargin,length;
	int here;
	
	leftMargin=0;
	length=strlen(string);
	while(1){
		rightMargin=leftMargin+lineLength;
		if(rightMargin>=length)return string;		/* successful completion */
		here=0;
		if(!here)for(i=leftMargin;i<rightMargin;i++)if(string[i]=='\n'){
			here=1;
			break;
		}
		if(!here)for(;i>=leftMargin;i--)if(string[i]==' ' || string[i]=='\n'){
			here=1;
			break;
		}
		if(!here)for(i=leftMargin;i<length;i++)if(string[i]==' ' || string[i]=='\n'){
			here=1;
			break;
		}
		if(!here)return string;
		string[i]='\n';
		leftMargin=i+1;
	}
}


Boolean PsychIsPsychMatchCaseSensitive(void)
{
	return(isPsychMatchCaseSensitive);
}


void PsychSetPsychMatchCaseSenstive(Boolean arg)
{
	isPsychMatchCaseSensitive=arg;
}


// Compare two strings for equality. Ignore case if Psychtoolbox preferences ignore case is set.
Boolean PsychMatch(char *s1,char *s2)
{
	char a;
	
	if(!isPsychMatchCaseSensitive){
		do{
			a=*s1++;
			if(tolower(a)!=tolower(*s2++))return 0;
		}while(a!='\0');
		return 1;
	}else 
		return strcmp(s1,s2)==0;

}



char *int2str(int num)
{
	static char numStr[256];
	
	sprintf(numStr, "%d", num);
	return(numStr);
}


int PsychIndexElementFrom2DArray(int mDim/*|Y|*/, int nDim/*|X|*/, int m/*y*/, int n/*x*/)
{
	
	return(n*mDim + m);  
}

int PsychIndexElementFrom3DArray(int mDim/*|Y|*/, int nDim/*|X|*/, int pDim/*|Z|*/, int m/*y*/, int n/*x*/, int p/*z*/)
{
	
	return(p*mDim*nDim + n*mDim + m);  //planeindex * planesize + columnindex * columsize + rowindex    
}


int PsychIndexPlaneFrom3DArray(int mDim, int nDim, int pDim, int planeIndex)
{
        return(planeIndex*mDim*nDim);
}

int maxInt(int a, int b)
{
	if(a>b)
		return(a);
	return(b);
}

/*
    isIntegerInDouble(double *value)
    
    If the value stored in the specified double does not have a fractional part an the value is within
    the bounds of the integet type then return TRUE.
*/
boolean PsychIsIntegerInDouble(double *value)
{
    return(*value >= INT_MIN && *value <= INT_MAX && floor(*value) == *value); 
}




	
