/*
 *    Psychtoolbox3/Source/Common/SCREENLoadNormalizedGammaTable.c
 *
 *    AUTHORS:
 *
 *    Allen.Ingling@nyu.edu             awi
 *    mario.kleiner.de@gmail.com        mk
 *
 *    PLATFORMS:
 *
 *    All.
 *
 *    HISTORY:
 *
 *    1/27/03  awi      Created.
 *    1/30/06  mk       Improved online help text.
 *
 *    DESCRIPTION:
 *
 *    Interface to the operating systems hardware gamma-table functions.
 */

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[oldtable, success] = Screen('LoadNormalizedGammaTable', windowPtrOrScreenNumber, table [, loadOnNextFlip][, physicalDisplay][, ignoreErrors]);";
//                          1         2                                             1                        2        3                 4                  5
static char synopsisString[] =
    "Load the gamma table of the specified screen or window 'windowPtrOrScreenNumber'.\n"
    "You need to pass the new hardware gamma table 'table' as a 'nrows' rows by 3 columns matrix. Each row corresponds to "
    "a single color index value in the framebuffer and contains the Red- green- and blue values "
    "to use for output. Column 1 is the red value, column 2 is the green value and column 3 is "
    "the blue value. Values have to be in range between 0.0 (for dark pixel) and 1.0 (for maximum intensity). "
    "Example: table(127,1)=0.67 would mean that the red color value 127 should be displayed with 67% of "
    "the maximum red-gun intensity, table(32, 3)=0.11 means that blue color value 32 should be displayed "
    "with 11% of the maximum blue-gun intensity. The range of values 0-1 gets mapped to the hardware with "
    "the accuracy attainable by the hardwares DAC's, typically between 8 and 10 bits.\n"
    "The required number of rows 'nrows' is typically 256 for consumer graphics cards.\n"
    "If the 'loadOnNextFlip' flag is set to a value < 2 for updating the graphics cards gamma table "
    "then the following limits apply: On MS-Windows, only gamma tables with 256 rows are accepted for the graphis card. "
    "On OS-X you can also pass 512, 1024, 2048, ..., 65535 rows instead of 256 rows, although this only "
    "makes sense for few selected applications. On Linux/X11 with some pro-graphics cards, "
    "e.g., some NVidia QuadroFX cards, you can pass more than 256 rows, similar to OS/X.\n"
    "If you provide the index of an onscreen window as 'ScreenNumber' and you set the (optional) "
    "flag 'loadOnNextFlip' to 1, then update of the gamma table will not happen immediately, but only at "
    "execution of the Screen('Flip', windowPtrOrScreenNumber) command. This allows to synchronize change of "
    "both the visual stimulus and change of the gamma table with each other and to the vertical retrace. If "
    "the flag is set to its default value of zero then update of the gamma table will happen at the next "
    "vertical retrace (or immediately if the graphics driver doesn't support sync to vertical retrace). "
    "A 'loadOnNextFlip' flag of 2 will load the provided table not into the hardware tables of your graphics "
    "card, but into the hardware tables of special display devices, like e.g., the Bits++ box. It can also "
    "be used to load clut's for color lookup table animation. Read the section about 'EnableCLUTMapping' "
    "in the 'help PsychImaging' for info on how to enable and use color lookup table animation.\n"
    "On MacOS-X, the optional 'physicalDisplay' flag can be set to 1, zero is the default. In this case, "
    "the 'windowPtrOrScreenNumber' argument (which then must be a real screen number, not a window index) "
    "selects among physically present display devices, instead of logical devices. "
    "This is important if you want to assign different gamma-tables to multiple displays in a 'clone' or "
    "'mirror mode' configuration, as there is only one logical display, but multiple physical displays, mirroring "
    "each other. Please note that screen numbering is different for physical vs. logical displays. For a list of "
    "physical display indices, call Screen('Screens', 1);\n"
    "On GNU/Linux X11, the optional 'physicalDisplay' parameter selects the video output to which the gamma "
    "table should be applied in multi-display mode. On Linux/X11 a screen can output to multiple video displays, "
    "therefore this parameter allows to setup individual gamma tables for each display. The default setting "
    "is -1, which means to apply the (same) gamma table to all outputs of the given screen.\n"
    "The optional 'ignoreErrors' parameter, if set to 1, will ask the function to continue in the case that the "
    "operating system rejected a gamma table update, instead of aborting your script, if you requested an immediate "
    "update via 'loadOnNextFlip' == 0. The optional 2nd return argument 'success' will be 1 on success, 0 on a "
    "rejected immediate update. 'ignoreErrors' will be ignored in case of deferred updates via 'loadOnNextFlip' > 0.\n"
    "On OSX and Linux/X11, this function takes arbitrary gamma-tables which makes it suitable for CLUT animation, "
    "although you should rather avoid CLUT animation, or use the PsychImaging(...'EnableCLUTMapping'...) method "
    "instead. CLUT animation nowadays is almost always the wrong approach. If you really need it, the PsychImaging() "
    "based method provides cross-platform compatibility and reliable timing for CLUT animation.\n"
    "On Microsoft Windows, only tables with monotonically increasing values are considered valid. Other tables "
    "get rejected by the operating system, so this is not suitable for CLUT animation, but only for linearizing "
    "or calibrating display devices. Microsoft Windows does impose additional unknown constraints beyond monotonicity, "
    "so generally anything not resembling a regular gamma calibration table may prove troublesome. Similar limitations "
    "currently apply to Linux when used with the Wayland display server.\n"
    "The function returns the old gamma table as optional 1st return argument and a success status code of 1 on success, "
    "0 on failure as 2nd optional return argument in case of an immediate update (ie 'loadOnNextFlip' == 0).\n";

static char seeAlsoString[] = "ReadNormalizedGammaTable";

PsychError SCREENLoadNormalizedGammaTable(void)
{
    int i, screenNumber, numEntries, inM, inN, inP, loadOnNextFlip, physicalDisplay, outputId, ignoreErrors;
    float *outRedTable, *outGreenTable, *outBlueTable, *inRedTable, *inGreenTable, *inBlueTable;
    double *inTable, *outTable;
    PsychWindowRecordType *windowRecord;

    //all subfunctions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(5));

    // Get optional physicalDisplay argument - It defaults to zero on OS/X, -1 on Linux:
    physicalDisplay = -1;
    PsychCopyInIntegerArg(4, FALSE, &physicalDisplay);

    // Read in the screen number:
    // On OS/X we also accept screen indices for physical displays (as opposed to active dispays).
    // This only makes a difference in mirror-mode, where there is only 1 active display, but that
    // corresponds to two physical displays which can have different gamma setting requirements:
    if ((PSYCH_SYSTEM == PSYCH_OSX) && (physicalDisplay > 0)) {
        PsychCopyInIntegerArg(1, TRUE, &screenNumber);
        if (screenNumber < 1)
            PsychErrorExitMsg(PsychError_user, "A 'screenNumber' that is smaller than one provided, although 'physicalDisplay' flag set. This is not allowed!");

        // Invert screenNumber as a sign its a physical display, not an active display:
        screenNumber = -1 * screenNumber;
    }
    else {
        PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);
    }

    if ((PSYCH_SYSTEM == PSYCH_LINUX) && (physicalDisplay > -1)) {
        // Affect one specific display output for given screen:
        outputId = physicalDisplay;
    }
    else {
        // Other OS'es, and Linux with default setting: Affect all outputs
        // for a screen.
        outputId = -1;
    }

    // Load and sanity check the input matrix:
    inM = -1; inN = -1; inP = -1;
    if (!PsychAllocInDoubleMatArg(2, FALSE, &inM,  &inN, &inP, &inTable)) {
        // Special case: Allow passing in an empty gamma table argument. This
        // triggers auto-load of identity LUT and setup of GPU for identity passthrough:
        inM = 0; inN = 0; inP = 0;
    }

    // Identity passthrouh setup requested? Empty LUT [] or LUT == -1 loads identity LUT and disables
    // colorspace conversions et al. Empty LUT [] also disables dithering, whereas LUT == -1 keeps
    // dithering untouched:
    if ((inM == 0 && inN == 0 && inP == 0) || (inM == 1 && inN == 1 && inP == 1 && inTable[0] < 0)) {
        // Yes. Try to enable it, return its status code:
        PsychAllocInWindowRecordArg(1, TRUE, &windowRecord);
        i = PsychSetGPUIdentityPassthrough(windowRecord, screenNumber, TRUE, (inM == 0) ? TRUE : FALSE);
        PsychCopyOutDoubleArg(1, FALSE, (double) i);

        // Done.
        return(PsychError_none);
    }
    else if (inM < 1) PsychErrorExitMsg(PsychError_user, "The gamma table must have at least one row.");

    // Sanity check dimensions:
    if((inN != 3) || (inP != 1)) PsychErrorExitMsg(PsychError_user, "The gamma table must have 3 columns (Red, Green, Blue).");

    // OS-X and Linux/X11 allow tables with other than 256 slots, Windows only ones with 256 slots.
    // OS/X either passes them to hw if in native size, or performs
    // software interpolation to convert it into native size. We allow any table size with 1 - x slots.
    // A table size of 1 row will have a special meaning. It interprets the 1 row of the table as gamma formula
    // min, max, gamma and lets the OS compute a corresponding gamma correction table.
    // A table size of zero rows will trigger an internal upload of an identity table via byte transfer.
    // On Linux/X11 we need to interpolate ourselves on non-matching table sizes.

    // Copy in optional loadOnNextFlip - flag. It defaults to zero. If provided
    // with a non-zero value, we will defer actual update of the gamma table to
    // the next bufferswap as initiated via Screen('Flip').
    loadOnNextFlip = 0;
    PsychCopyInIntegerArg(3, FALSE, &loadOnNextFlip);

    if (loadOnNextFlip>0) {
        if ((PSYCH_SYSTEM == PSYCH_OSX) && (physicalDisplay > 0)) PsychErrorExitMsg(PsychError_user, "Non-zero 'loadOnNextFlip' flag not allowed if 'physicalDisplays' flag is non-zero!");
        if ((PSYCH_SYSTEM == PSYCH_LINUX) && (physicalDisplay > -1)) PsychErrorExitMsg(PsychError_user, "Non-zero 'loadOnNextFlip' flag not allowed if 'physicalDisplays' setting is positive!");

        // Allocate tables in associated windowRecord: We will update during next
        // Flip operation for specified windowRecord.
        PsychAllocInWindowRecordArg(1, TRUE, &windowRecord);

        // Sanity checks:
        if (!PsychIsOnscreenWindow(windowRecord)) PsychErrorExitMsg(PsychError_user, "Target window for gamma table upload is not an onscreen window!");
        if (windowRecord->inRedTable && loadOnNextFlip!=2) PsychErrorExitMsg(PsychError_user, "This window has already a new gamma table assigned for upload on next Flip!");

        if (windowRecord->inRedTable && windowRecord->inTableSize != inM) {
            free(windowRecord->inRedTable); windowRecord->inRedTable = NULL;
            free(windowRecord->inGreenTable); windowRecord->inGreenTable = NULL;
            free(windowRecord->inBlueTable); windowRecord->inBlueTable = NULL;
        }

        if (windowRecord->inRedTable == NULL) {
            // Allocate persistent memory:
            inRedTable=malloc(sizeof(float) * inM);
            inGreenTable=malloc(sizeof(float) * inM);
            inBlueTable=malloc(sizeof(float) * inM);

            // Assign the pointers to the windowRecord:
            windowRecord->inRedTable = inRedTable;
            windowRecord->inGreenTable = inGreenTable;
            windowRecord->inBlueTable = inBlueTable;
            windowRecord->inTableSize = inM;
        }
        else {
            inRedTable = windowRecord->inRedTable;
            inGreenTable = windowRecord->inGreenTable;
            inBlueTable = windowRecord->inBlueTable;
        }

        windowRecord->loadGammaTableOnNextFlip = (loadOnNextFlip == 1) ? 1 : 0;
    }
    else {
        // Allocate temporary tables: We will update immediately.
        inRedTable=PsychMallocTemp(sizeof(float) * inM);
        inGreenTable=PsychMallocTemp(sizeof(float) * inM);
        inBlueTable=PsychMallocTemp(sizeof(float) * inM);
    }

    for(i=0;i<inM;i++){
        inRedTable[i]=(float)inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i, 0, 0)];
        inGreenTable[i]=(float)inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i, 1, 0)];
        inBlueTable[i]=(float)inTable[PsychIndexElementFrom3DArray(inM, 3, 0, i, 2, 0)];

        if(inRedTable[i]>1 || inRedTable[i]< 0 || inGreenTable[i] > 1 || inGreenTable[i] < 0 || inBlueTable[i] >1 || inBlueTable[i] < 0)
            PsychErrorExitMsg(PsychError_user, "Gamma Table Values must be in interval 0 =< x =< 1");
    }

    if (loadOnNextFlip < 2) {
        //first read the existing gamma table so we can return it.
        PsychReadNormalizedGammaTable(screenNumber, outputId, &numEntries, &outRedTable, &outGreenTable, &outBlueTable);
        PsychAllocOutDoubleMatArg(1, FALSE, numEntries, 3, 0, &outTable);

        for(i=0;i<numEntries;i++){
            outTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 0, 0)]=(double)outRedTable[i];
            outTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 1, 0)]=(double)outGreenTable[i];
            outTable[PsychIndexElementFrom3DArray(numEntries, 3, 0, i, 2, 0)]=(double)outBlueTable[i];
        }
    }

    // Now set the new gamma table in case of an immediate load request:
    if (loadOnNextFlip == 0) {
        if (0 == PsychLoadNormalizedGammaTable(screenNumber, outputId, inM, inRedTable, inGreenTable, inBlueTable)) {
            // Failed. To bail or not to bail?
            if (!PsychCopyInIntegerArg(5, FALSE, &ignoreErrors) || !ignoreErrors)
                PsychErrorExitMsg(PsychError_user, "Failed to upload the gamma table.");
            PsychCopyOutDoubleArg(2, FALSE, 0);
        }
        else {
            // Success:
            PsychCopyOutDoubleArg(2, FALSE, 1);
        }
    }
    else {
        // Deferred update. Assume success:
        PsychCopyOutDoubleArg(2, FALSE, 1);
    }

    return(PsychError_none);
}
