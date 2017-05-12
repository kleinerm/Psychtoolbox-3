/*
    PsychToolbox3/Source/windows/Screen/PsychWindowGlue.h

    PLATFORMS:

        This is the Windows version only.

    AUTHORS:

        Mario Kleiner   mk      mario.kleiner.de@gmail.com

    DESCRIPTION:

        Functions in this file comprise an abstraction layer for probing and controlling window state, except for window content.

        Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes()
        should be platform-neutral, despite that the calls in OS X and Windows to detect available pixel sizes are different.  Platform
        specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, PsychWindowTextClue.

*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychWindowGlue
#define PSYCH_IS_INCLUDED_PsychWindowGlue

#include "Screen.h"

psych_bool PsychRealtimePriority(psych_bool enable_realtime);
psych_bool PsychOSOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType *windowRecord, int numBuffers, int stereomode, int conserveVRAM);
void    PsychOSCloseOnscreenWindow(PsychWindowRecordType *windowRecord);
void    PsychOSCloseWindow(PsychWindowRecordType *windowRecord);
void    PsychOSFlipWindowBuffers(PsychWindowRecordType *windowRecord);
void    PsychOSSetVBLSyncLevel(PsychWindowRecordType *windowRecord, int swapInterval);
void    PsychOSSetGLContext(PsychWindowRecordType *windowRecord);
void    PsychOSUnsetGLContext(PsychWindowRecordType *windowRecord);
void    PsychOSSetUserGLContext(PsychWindowRecordType *windowRecord, psych_bool copyfromPTBContext);
double  PsychOSGetVBLTimeAndCount(PsychWindowRecordType *windowRecord, psych_uint64* vblCount);
void    PsychGetMouseButtonState(double* buttonArray);
psych_bool PsychOSGetPresentationTimingInfo(PsychWindowRecordType *windowRecord, psych_bool postSwap, unsigned int flags, psych_uint64* onsetVBLCount, double* onsetVBLTime, psych_uint64* frameId, double* compositionRate, int fullStateStructReturnArgPos);
psych_bool PsychOSSetPresentParameters(PsychWindowRecordType *windowRecord, psych_uint64 targetVBL, unsigned int queueLength, double rateDuration);
int     PsychOSIsDWMEnabled(int screenNumber);
psych_bool PsychOSSetupFrameLock(PsychWindowRecordType *masterWindow, PsychWindowRecordType *slaveWindow);
psych_int64 PsychOSScheduleFlipWindowBuffers(PsychWindowRecordType *windowRecord, double tWhen, psych_int64 targetMSC, psych_int64 divisor, psych_int64 remainder, unsigned int specialFlags);
psych_int64 PsychOSGetSwapCompletionTimestamp(PsychWindowRecordType *windowRecord, psych_int64 targetSBC, double* tSwap);
void    PsychOSProcessEvents(PsychWindowRecordType *windowRecord, int flags);
double  PsychOSAdjustForCompositorDelay(PsychWindowRecordType *windowRecord, double targetTime, psych_bool onlyForCalibration);
psych_bool PsychOSConstrainPointer(PsychWindowRecordType *windowRecord, psych_bool constrain, PsychRectType rect);

//end include once
#endif
