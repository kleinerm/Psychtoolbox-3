/* 
    /PsychSourceGL/Source/Common/Eyelink/EyelinkClockBase.c

    PROJECTS: Eyelink 

    AUTHORS:
        brian@sr-research.com           br

    PLATFORMS: all
    
    HISTORY:
        13/11/24    br                  Initial implementation of eyelink_set_clock_base & eyelink_get_clock_base

    TARGET LOCATION:
        EyelinkToolbox
*/

#include "PsychEyelink.h"

static char useString[] = "[old_clock_base =] Eyelink('ClockBase' [, clock_base])";

static char synopsisString[] =
   "Gets/sets the clock base for the local time access. This\n"
   "changes the behaviour of the time functions.\n"
   "Returns 'old_clock_base'. Optional 'clock_base' to set:\n"
   "\t0: SR_CLOCK_BASE_APPLICATION_LOCAL\n"
   "\t\tSR_CLOCK_BASE_APPLICATION_LOCAL is the default. The time stamp starts\n"
   "\t\tfrom the beginning of the application. Reading this time across multiple\n"
   "\t\tapplication on the same computer will not provide meaningful result as\n"
   "\t\tthe time bases are different.\n"
   "\t1: SR_CLOCK_BASE_MACHINE_LOCAL\n"
   "\t\tThe time stamp starts from the boot time of the machine or a random point.\n"
   "\t\tReading this time across multiple applications will give comparable time.\n"
   "\t\tThis selection is useful, when you have multiple applications that save\n"
   "\t\tdata or send messages to the tracker with a local time stamp.\n"
   "\t\tRemarks: This selection may lead to premature clock roll over.\n";

static char seeAlsoString[] = "";

PsychError EyelinkClockBase(void)
{
   EL_CLOCK_BASE old_clock_base;
   int clock_base = -1;

   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(1));
   PsychErrorExit(PsychRequireNumInputArgs(0));
   PsychErrorExit(PsychCapNumOutputArgs(1));
 
   old_clock_base =  eyelink_get_clock_base();

   if (PsychGetNumInputArgs() == 1)
   {
      PsychCopyInIntegerArg(1, kPsychArgOptional, &clock_base);
      if (clock_base < 0 | clock_base > 1) PsychErrorExitMsg(PsychError_user, "Invalid value for clock_base provided. Valid are values of 0 or 1.");

      // Return current/old clock_base:
      PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) old_clock_base);

      // Set new level, if one was provided:
      if (clock_base > -1) eyelink_set_clock_base(clock_base);
   }

   PsychCopyOutDoubleArg(1, TRUE, old_clock_base);

   return(PsychError_none);
}
