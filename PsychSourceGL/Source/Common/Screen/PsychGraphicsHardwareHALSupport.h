/*
    PsychToolbox3/Source/Common/Screen/PsychGraphicsHardwareHALSupport.h

    AUTHORS:

        mario.kleiner.de@gmail.com        mk

    PLATFORMS:

        All. However with dependencies on OS specific glue-layers which are mostly Linux/OSX for now...

    HISTORY:

    01/12/2008    mk        Created.

    DESCRIPTION:

    This file is a container for miscellaneous routines that take advantage of specific low level
    features of graphics/related hardware and the target operating system to achieve special tasks.

    Most of the routines here are more tied to specific displays (screens) than to windows and usually
    only a subset of these routines is available for a specific system configuration with a specific
    model of graphics card. Other layers of PTB should not rely on these routines being supported on
    a given system config and should be prepared to have fallback-implementations.

    Many of the features are experimental in nature!

    For more infos, see the corresponding .c implementation file!

*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychGraphicsHardwareHALSupport
#define PSYCH_IS_INCLUDED_PsychGraphicsHardwareHALSupport

// We need to include Screen.h for definition of windowRecords,
// but have some include dependency issues which we "fix" / band-aid
// by setting this special #define to prevent some definitions
// inside the WindowBank.h -- Very ugly and dangerous if not done
// with lots of care!
#define PSYCH_DONT_INCLUDE_TEXTATTRIBUTES_IN_WINDOWRECORD 1
#include "Screen.h"
#undef PSYCH_DONT_INCLUDE_TEXTATTRIBUTES_IN_WINDOWRECORD

// Internal helper routines:

// Routines exposed to PTB core:

// (Try to) synchronize display refresh cycles of multiple displays:
PsychError PsychSynchronizeDisplayScreens(int *numScreens, int* screenIds, int* residuals, unsigned int syncMethod, double syncTimeOut, int allowedResidual);

// Enable/Disable native 10 bpc RGB framebuffer modes.
psych_bool PsychEnableNative10BitFramebuffer(PsychWindowRecordType* windowRecord, psych_bool enable);

// Keep 10 bpc framebuffer mode enabled across unwanted changes caused by gfx-driver: Must be called after end-of-scene marker commands, e.g., after
// each glClear() command! Is a no-op if fixup not needed, so cheap to call.
void PsychFixupNative10BitFramebufferEnableAfterEndOfSceneMarker(PsychWindowRecordType* windowRecord);

// Store current frontbuffer addresses (inside GPU) to windowRecord: Called from PsychFlipWindowBuffers():
void PsychStoreGPUSurfaceAddresses(PsychWindowRecordType* windowRecord);

// Wait until a bufferswap is complete or bufferswap at next retrace is certain:
psych_bool PsychWaitForBufferswapPendingOrFinished(PsychWindowRecordType* windowRecord, double* timestamp, int *beamposition);
unsigned int PsychGetNVidiaGPUType(PsychWindowRecordType* windowRecord);

// Map PTB screenId to GPU output headId : rankId selects which output head (0 = primary).
int PsychScreenToHead(int screenId, int rankId);
// Change mapping of a PTB screenId to GPU output headId:
void PsychSetScreenToHead(int screenId, int headId, int rankId);

// Setup initial mapping for 'numDisplays' displays:
void PsychInitScreenToHeadMappings(int numDisplays);

// Map PTB screenId and output head id to the index of the associated low-level crtc scanout engine of the GPU: rankId selects which output head (0 = primary).
int PsychScreenToCrtcId(int screenId, int rankId);
void PsychSetScreenToCrtcId(int screenId, int crtcId, int rankId);
void PsychResetCrtcIdUserOverride(void);

// Get/Set corrective offsets for scanout position queries on certain GPU's:
void PsychGetBeamposCorrection(int screenId, int *vblbias, int *vbltotal);
void PsychSetBeamposCorrection(int screenId, int vblbias, int vbltotal);

// Control bit depth control and dithering on digital display output encoder:
psych_bool PsychSetOutputDithering(PsychWindowRecordType* windowRecord, int screenId, unsigned int ditherEnable);

// Control identity passthrough of framebuffer 8 bpc pixel values to encoders/connectors:
unsigned int PsychSetGPUIdentityPassthrough(PsychWindowRecordType* windowRecord, int screenId, psych_bool passthroughEnable, psych_bool changeDithering);

// Try to auto-detect screen to head mappings if possible and not yet overriden by usercode:
void PsychAutoDetectScreenToHeadMappings(int maxHeads);

// Try to check the hardware itself if a pageflip is completed or pending for a windowRecord:
psych_bool PsychGetCurrentGPUSurfaceAddresses(PsychWindowRecordType* windowRecord, psych_uint64* primarySurface, psych_uint64* secondarySurface, psych_bool* updatePending);
int PsychIsGPUPageflipUsed(PsychWindowRecordType* windowRecord);

// End of routines.

#endif // include once.
