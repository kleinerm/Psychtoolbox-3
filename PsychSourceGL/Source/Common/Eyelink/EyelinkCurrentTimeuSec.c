/* 
    /PsychSourceGL/Source/Common/Eyelink/EyelinkCurrentTimeuSec.c

    PROJECTS: Eyelink 

    AUTHORS:
        brian@sr-research.com           br

    PLATFORMS: all
    
    HISTORY:
        19/11/24    br                  Initial implementation of current_double_usec

    TARGET LOCATION:
        EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "[current_time_usec] = Eyelink('CurrentTime_uSec')";

static char synopsisString[] =
   "Returns the current microsecond as double (56 bits) since the\n"
   "initialization. Equivalent to current_micro() and current_usec().\n"
   "The current microsecond as a double value since the initialization\n"
   "of the library, modulo 2^32.\n";

static char seeAlsoString[] = "";

PsychError EyelinkCurrentTimeuSec(void)
{
   double current_time_usec;

   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(0));
   PsychErrorExit(PsychRequireNumInputArgs(0));
   PsychErrorExit(PsychCapNumOutputArgs(1));

   // Verify eyelink is up and running
   EyelinkSystemIsConnected();
   EyelinkSystemIsInitialized();

   current_time_usec = current_double_usec();

   PsychCopyOutDoubleArg(1, TRUE, current_time_usec);

   return(PsychError_none);
}
