/*

  Psychtoolbox3/Source/Common/SCREENNull.c		

  

  AUTHORS:

  Allen.Ingling@nyu.edu		awi 

  

  PLATFORMS:	

  This file should build on any platform. 



  HISTORY:

  11/26/02  awi		Created. 

 

  DESCRIPTION:

  

  

  

  TO DO:

  

  

  



*/





#include "Screen.h"





static char useString[] = "[[value1], [value2] ..]=SCREEN('Null',[value1],[value2],...);";

static char synopsisString[] = 

	"Null copies all double and char arguments supplied as inputs to respective outputs. ";

static char seeAlsoString[] = "";

	 



PsychError SCREENNull(void) 

{

	const double defaultMatrix[] = {1.1, 1.2, 1.3, 1.4, 2.1, 2.2, 2.3, 2.4};

	const double defaultM=2, defaultN=4; 

	double tempValue; 

	double *array;

	int i, m,n, p, numInArgs, numOutArgs, numNamedOutArgs;

	char *str;

	PsychArgFormatType format;

	const char defaultString[] = "I am the default string\n";

	


return(PsychError_none);


	//all sub functions should have these two lines

	PsychPushHelp(useString, synopsisString, seeAlsoString);

	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};



	//demonstrate how we find the function and subfunction names

	//printf("Psychtoolbox function: %s, subfunction %s\n", PsychGetModuleName(), PsychGetFunctionName() );



	//copy all the input argument to their outputs if we have doubles, if not error.  

	numInArgs = PsychGetNumInputArgs();

	numOutArgs = PsychGetNumOutputArgs();

	numNamedOutArgs = PsychGetNumNamedOutputArgs();

	

	PsychErrorExit(PsychCapNumOutputArgs(numInArgs));

	

	

	/*

	printf("number of input arguments: %d\n", numInArgs);

	printf("number of output arguments: %d\n", numOutArgs);

	printf("number of named output arguments: %d\n", numNamedOutArgs);

	*/

	

		

	

	//iterate over each of the supplied inputs.  If the input is a two-dimensional array 

	//of doubles or a character array, then copy it to the output.  

	for(i=1;i<=numInArgs;i++){

		format = PsychGetArgType(i);

		switch(format){

			case PsychArgType_double:

				if(PsychGetArgM(i)==1 && PsychGetArgN(i)==1){

					tempValue=i;  //if 1x1 double then the default return value is the arg position.

					PsychCopyInDoubleArg(i, FALSE, &tempValue);

					PsychCopyOutDoubleArg(i, FALSE, tempValue);

				}else{

					PsychAllocInDoubleMatArg(i, FALSE, &m, &n, &p, &array);

					PsychCopyOutDoubleMatArg(i, FALSE, m, n, p, array);

				}

				break;

			case PsychArgType_char:

				str=NULL; //This tells PsychGetCharArg() to use its own (volatile) memory. 

				PsychAllocInCharArg(i, FALSE, &str); 

				PsychCopyOutCharArg(i, FALSE, str);

				break;

			case PsychArgType_default:

				PsychCopyOutCharArg(i, FALSE, defaultString);

				break;

		}

	}

		

	return(PsychError_none);

}










