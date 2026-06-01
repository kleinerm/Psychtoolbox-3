/* 
    /PsychSourceGL/Source/Common/Eyelink/EyelinkSetOptions.c

    PROJECTS: Eyelink 

    AUTHORS:
        brian@sr-research.com           br

    PLATFORMS: all
    
    HISTORY:
        13/11/24    br                  Initial implementation of eyelink_set_options

    TARGET LOCATION:
        EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "[status =] Eyelink('SetOptions', 'options')";

static char synopsisString[] =
   "Accepts 'options' with text specifying options. Currently long edf\n"
   "filename support is supported, for example:\n"
   "\tEyelink('SetOptions', 'longname=1')\n"
   "where calling this before Eyelink('OpenFile', ...) will enable long\n"
   "filename exceeding 8 char limitation\n"
   "Returns 1 if success, else error code -1 for parsing errors and 0 for no options.\n";

static char seeAlsoString[] = "";

PsychError EyelinkSetOptions(void)
{
   char *options;
   int status;

   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(1));
   PsychErrorExit(PsychRequireNumInputArgs(1));
   PsychErrorExit(PsychCapNumOutputArgs(1));

   // Verify eyelink is up and running
   EyelinkSystemIsConnected();
   EyelinkSystemIsInitialized();

   PsychAllocInCharArg(1, TRUE, &options);

   status = eyelink_set_options(options);

   PsychCopyOutDoubleArg(1, TRUE, status);

   return(PsychError_none);
}
