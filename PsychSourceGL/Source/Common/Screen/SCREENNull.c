/*

  Psychtoolbox3/Source/Common/SCREENNull.c

  AUTHORS:

  Allen.Ingling@nyu.edu		awi

  PLATFORMS:

  This file should build on any platform.

  HISTORY:

  11/26/02  awi		Created.

  DESCRIPTION:

  Arbitrary code to be executed for debugging/testing/hacking PTB. This subfunction is not
  documented or mentioned in official docs and only here for the purpose of internal testing
  and as a container for quick & dirty hacks and tests by the core developers.

  Therefore its syntax and semantics can change in any way, anytime without warning.

*/

#include "Screen.h"
#include "PsychGraphicsCardRegisterSpecs.h"

// Defined in PsychGraphicsHardwareHALSupport.c, but accessed here:
extern unsigned int crtcoff[kPsychMaxPossibleCrtcs];

#if PSYCH_SYSTEM == PSYCH_OSX
#include <IOKit/IOKitLib.h>
#include <sys/time.h>
#endif

static char useString[] = "[[value1], [value2] ..]=SCREEN('Null',[value1],[value2],...);";
static char synopsisString[] =
    "Special test function for Psychtoolbox testing and developments. Normal users: DO NOT USE!\n";
static char seeAlsoString[] = "";

PsychError SCREENNull(void)
{
    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    return(PsychError_none);

/*
    #if PSYCH_SYSTEM == PSYCH_LINUX
        unsigned int regOffset, value, hi, lo;
        PsychCopyInIntegerArg(1, TRUE, (int *) &regOffset);
        value = PsychOSKDReadRegister(0, regOffset, NULL);

        hi = value >> 16;
        lo = value & 0xffff;

        if (PsychPrefStateGet_Verbosity() > 2) printf("%p :: hi = %i , lo = %i , val = %i\n", (void*) (size_t) regOffset, hi, lo, value);

        PsychCopyOutDoubleArg(1, FALSE, (double) value);
        PsychCopyOutDoubleArg(2, FALSE, (double) hi);
        PsychCopyOutDoubleArg(3, FALSE, (double) lo);

        return(PsychError_none);
    #endif
*/
}
