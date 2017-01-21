/*
    PsychToolbox3/Source/Windows/Screen/PsychScreenGlue.h

    PLATFORMS:

        This is the Windows version only.

    AUTHORS:

        Mario Kleiner   mk      mario.kleiner.de@gmail.com

    HISTORY:

        12/27/05        mk      Wrote it. Derived from Allens OSX version.

    DESCRIPTION:

        Functions in this file comprise an abstraction layer for probing and controlling screen state.

        Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes()
        should be platform-neutral, despite that the calls in OS X and Windows to detect available pixel sizes are
        different.  The platform specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue,
        PsychWindowTextClue.

        In addition to glue functions for windows and screen there are functions which implement shared functionality between between Screen commands,
        such as ScreenTypes.c and WindowBank.c.

*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychScreenGlue
#define PSYCH_IS_INCLUDED_PsychScreenGlue

#include "Screen.h"

//functions from PsychScreenGlue
void                    InitializePsychDisplayGlue(void);
void                    PsychCleanupDisplayGlue(void);
void                    PsychGetCGDisplayIDFromScreenNumber(CGDirectDisplayID *displayID, int screenNumber);
void                    PsychCaptureScreen(int screenNumber);
void                    PsychReleaseScreen(int screenNumber);
psych_bool              PsychIsScreenCaptured(int screenNumber);
int                     PsychGetNumDisplays(void);
void                    PsychGetScreenDepths(int screenNumber, PsychDepthType *depths);
int                     PsychGetAllSupportedScreenSettings(int screenNumber, int outputId, long** widths, long** heights, long** hz, long** bpp);
psych_bool              PsychCheckVideoSettings(PsychScreenSettingsType *setting);
void                    PsychGetScreenDepth(int screenNumber, PsychDepthType *depth);   //dont' use this and get rid  of it.
int                     PsychGetScreenDepthValue(int screenNumber);
int                     PsychGetNominalFramerate(int screenNumber);
void                    PsychGetScreenPixelSize(int screenNumber, long *width, long *height);
void                    PsychGetScreenSize(int screenNumber, long *width, long *height);
void                    PsychGetGlobalScreenRect(int screenNumber, double *rect);
void                    PsychGetScreenRect(int screenNumber, double *rect);
void                    PsychGetDisplaySize(int screenNumber, int *width, int *height);
PsychColorModeType      PsychGetScreenMode(int screenNumber);
int                     PsychGetDacBitsFromDisplay(int screenNumber);        //from display, not from preferences
void                    PsychGetScreenSettings(int screenNumber, PsychScreenSettingsType *settings);
psych_bool              PsychSetScreenSettings(psych_bool cacheSettings, PsychScreenSettingsType *settings);
psych_bool              PsychRestoreScreenSettings(int screenNumber);
void                    PsychHideCursor(int screenNumber, int deviceIdx);
void                    PsychShowCursor(int screenNumber, int deviceIdx);
void                    PsychPositionCursor(int screenNumber, int x, int y, int deviceIdx);
void                    PsychReadNormalizedGammaTable(int screenNumber, int outputId, int *numEntries, float **redTable, float **greenTable, float **blueTable);
unsigned int            PsychLoadNormalizedGammaTable(int screenNumber, int outputId, int numEntries, float *redTable, float *greenTable, float *blueTable);
char*                   PsychGetDisplayDeviceName(int screenNumber);
int                     PsychGetDisplayBeamPosition(CGDirectDisplayID cgDisplayId, int screenNumber);
psych_bool              PsychOSIsKernelDriverAvailable(int screenId);

// Return identifying information about GPU for a given screen screenNumber:
psych_bool PsychGetGPUSpecs(int screenNumber, int* gpuMaintype, int* gpuMinortype, int* pciDeviceId, int* numDisplayHeads);
const char*             PsychOSDisplayDPITrouble(unsigned int screenNumber);

//end include once
#endif
