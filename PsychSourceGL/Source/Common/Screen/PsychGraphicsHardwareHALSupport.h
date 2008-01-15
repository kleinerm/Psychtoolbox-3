/*
	PsychToolbox3/Source/Common/Screen/PsychGraphicsHardwareHALSupport.h		

	AUTHORS:
	
		mario.kleiner@tuebingen.mpg.de		mk

	PLATFORMS:	
	
		All. However with dependencies on OS specific glue-layers which are mostly Linux/OSX for now...

	HISTORY:
	
	01/12/2008	mk		Created.
	
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
PsychError	PsychSynchronizeDisplayScreens(int *numScreens, int* screenIds, int* residuals, unsigned int syncMethod, double syncTimeOut, int allowedResidual);

// Enable/Disable native 10 bpc RGB framebuffer modes.
boolean		PsychEnableNative10BitFramebuffer(PsychWindowRecordType* windowRecord, boolean enable);

// Keep 10 bpc framebuffer mode enabled across unwanted changes caused by gfx-driver: Must be called after end-of-scene marker commands, e.g., after
// each glClear() command! Is a no-op if fixup not needed, so cheap to call.
void		PsychFixupNative10BitFramebufferEnableAfterEndOfSceneMarker(PsychWindowRecordType* windowRecord);

// End of routines.

#endif // include once.
