/* 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkMessage.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		15/06/06  fwc		Adapted from early alpha version by emp.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "[status =] Eyelink('Message', 'formatstring', [...])";

static char synopsisString[] = 
   "link message formatting use just like printf()\n" 
   " Only chars and ints allowed in arguments!\n"
   "returns any send error";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKmessage
PURPOSE:
  uses int eyemsg_printf(char *fmt, ...)
  This sends a text message to the Eyelink tracker, which
  timestamps it and writes it to the EDF data file. Messages
  are useful for recording trail conditions, subject responses.
  or the time of important events. This function is used with
  the same formatting methods as printf(), allowing numbers to be
  included. Avoid"\n" at end of messages.
  RETURNS: 0 if OK, else errorcode. */
  	
PsychError EyelinkMessage(void)
{
   int i, numInArgs;
   int status = -1;
   void **args_ptr = NULL; /* argument list, used as a va_list */
   char s[256];
   char *formatString;
   PsychArgFormatType format;
   double tempValue;
   char *tempString = NULL;
   
   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   // PsychErrorExit(PsychCapNumInputArgs(1));
   PsychErrorExit(PsychRequireNumInputArgs(1));
   PsychErrorExit(PsychCapNumOutputArgs(1));
   
   	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
   
   PsychAllocInCharArg(1, TRUE, &formatString);
   numInArgs = PsychGetNumInputArgs();   

   if (numInArgs > 1)
   {
      args_ptr = (void **)mxMalloc((numInArgs-1) * sizeof(char *));
      //iterate over each of the supplied inputs.  
      for(i=2;i<=numInArgs;i++){
         format = PsychGetArgType(i);
         switch(format){
            case PsychArgType_double:
               if(PsychGetArgM(i)==1 && PsychGetArgN(i)==1){
                  PsychCopyInDoubleArg(i, TRUE, &tempValue);
                  args_ptr[i-2] = (void *) (int) tempValue;
               }
               else
               {
                  PsychGiveHelp();
                  return(PsychError_user);
               }
               break;
            case PsychArgType_char:
               args_ptr[i-2] = NULL;
               PsychAllocInCharArg(i, TRUE, &tempString); 
               args_ptr[i-2] = tempString;
               break;
            default:
               PsychGiveHelp();
               return(PsychError_user);
               break;
         }
      }
   }

   vsprintf(s, formatString, (va_list)args_ptr);

   status = eyemsg_printf(s);

   if (args_ptr != NULL)
      mxFree(args_ptr);

   /* if there is an output variable available, assign eyecmd_printf status to it.   */
   PsychCopyOutDoubleArg(1, FALSE, status);
   
   return(PsychError_none);
}


