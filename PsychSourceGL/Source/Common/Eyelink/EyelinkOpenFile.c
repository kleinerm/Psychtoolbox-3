/*
    Psychtoolbox-3/PsychSourceGL/Source/Common/Eyelink/EyelinkOpenFile.c

    PROJECTS: Eyelink

    AUTHORS:
        f.w.cornelissen@rug.nl            fwc
        E.Peters@ai.rug.nl                emp

    PLATFORMS:  All.

    HISTORY:

        18/10/02    fwc        removed superfluous message when succesfully opening file
        15/06/06    fwc        Adapted from early alpha version by emp.
*/

#include "PsychEyelink.h"

static char useString[] = "[status =] Eyelink('OpenFile', filename [, dontOpenExisting=0])";
static char synopsisString[] =
    "Opens an EDF file 'filename' on the tracker computer, closes any existing file.\n"
    "If a file of the name 'filename' already exists, it will be overwritten!\n"
    "The optional flag 'dontOpenExisting' only exists for backwards compatibility, and to warn "
    "users to *not* use or rely on this flag in their scripts, as it never ever worked as intended! "
    "If it is now set to a non-zero value, then Eyelink will abort with an error message to that "
    "effect, to protect users from potential data loss. The intended purpose of setting the flag "
    "to a non-zero value was that a data file 'filename' would only be opened, and thereby "
    "created, if it would not already exist. Otherwise the function would abort with an error. "
    "According to authoritative feedback from SR-Research, this does not and did not ever work, "
    "and it is currently impossible to implement this functionality."
    "Returns 0 if success, else error code" ;

static char seeAlsoString[] = "";

/*
    ROUTINE: EyelinkOpenFile
    PURPOSE:

    uses INT16 open_data_file(char *name);
    Opens an EDF file on tracker hard disk, closes any existing file
    Returns 0 if success, else error code
*/
PsychError EyelinkOpenFile(void)
{
   int iOpenFileStatus = -1;
   int dontOpenExisting = 0;
   char *filename;

   //all sub functions should have these two lines
   PsychPushHelp(useString, synopsisString, seeAlsoString);
   if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

   //check to see if the user supplied superfluous arguments
   PsychErrorExit(PsychCapNumInputArgs(2));
   PsychErrorExit(PsychRequireNumInputArgs(1));
   PsychErrorExit(PsychCapNumOutputArgs(1));

   // Verify eyelink is up and running
   EyelinkSystemIsConnected();
   EyelinkSystemIsInitialized();

   PsychAllocInCharArg(1, TRUE, &filename);

   // Should we only open new files, not open - and overwrite - existing ones?
   if (PsychCopyInIntegerArg(2, FALSE, &dontOpenExisting) && dontOpenExisting) {
       // Yes. Abort with error, as this functionality is impossible to implement with
       // all current and all past Eyelink runtimes and tracker host software:
       printf("Eyelink Openfile: ERROR: The 'dontOpenExisting' flag was set to a non-zero value, which is unsupportable! Aborting.\n");
       PsychErrorExitMsg(PsychError_user, "Unsupported functionality requested. Read the help 'Eyelink OpenFile?' for explanation.");
   }

   iOpenFileStatus = open_data_file(filename);
   if (iOpenFileStatus!=0)
      mexPrintf("Eyelink openfile: Cannot create EDF file '%s' errorcode : %d\n", filename, iOpenFileStatus);

   /* if there is an output variable available, assign iOpenFileStatus to it. */
   PsychCopyOutDoubleArg(1, FALSE, iOpenFileStatus);

   return(PsychError_none);
}
