/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/PsychEyelink.h
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/21/05  cdb		Created.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#ifndef PSYCH_IS_INCLUDED_Eyelink
#define PSYCH_IS_INCLUDED_Eyelink

// PsychToolbox Includes
#include "Psych.h"

// Eyelink Includes
#include "eyelink.h"
// MK: core_expt.h must be included for M$-Windows build:
#include "core_expt.h"

#if PSYCH_SYSTEM == PSYCH_WINDOWS
// These not defined on non-OS/X:
typedef UINT8 UInt8;
typedef UINT16 UInt16;
typedef UINT32 UInt32;
typedef INT8 Int8;
typedef INT16 Int16;
typedef INT32 Int32;
#endif

#if PSYCH_SYSTEM == PSYCH_LINUX
typedef psych_uint8 UInt8;
typedef psych_uint16 UInt16;
typedef psych_uint32 UInt32;
#endif

/////////////////////////////////////////////////////////////////////////
// Global variables used throughout eyelink C files

extern int		giSystemInitialized;

/////////////////////////////////////////////////////////////////////////
//		Eyelink Function prototypes

// Defined in EyelinkCreateDataStructs.c
mxArray *CreateMXFSample(const FSAMPLE *fs);
mxArray *CreateMXFSampleRaw(const FSAMPLE_RAW *fs);
mxArray *CreateMXISample(const ISAMPLE *is);
mxArray *CreateMXFEvent(const FEVENT *fe);
mxArray *CreateMXIEvent(const IEVENT *ie);

// Defined in PsychEyelink.c
int Verbosity(void);
PsychError	EyelinkSystemIsConnected(void);
PsychError	EyelinkSystemIsInitialized(void);
void PsychEyelink_init_core_graphics(const char* callback);
void PsychEyelink_uninit_core_graphics(void);
void PsychEyelink_TestEyeImage(void);
void PsychEyelink_dumpHookfunctions(void);

//callback functions and supporting functions, also defined in PsychEyelink.c
static INT16 ELCALLBACK  PsychEyelink_setup_image_display(INT16 width, INT16 height);
static void ELCALLBACK   PsychEyelink_exit_image_display(void);
static void ELCALLBACK   PsychEyelink_set_image_palette(INT16 ncolors, byte r[130], byte g[130], byte b[130]);
static void ELCALLBACK   PsychEyelink_draw_image_line(INT16 width, INT16 line, INT16 totlines, byte *pixels);

static INT16  ELCALLBACK PsychEyelink_setup_cal_display(void);
static void ELCALLBACK   PsychEyelink_clear_display(void);
static void ELCALLBACK   PsychEyelink_draw_cal_target(INT16 x, INT16 y);
static void ELCALLBACK   PsychEyelink_image_title(INT16 threshold, char *title);
static INT16 ELCALLBACK  PsychEyelink_get_input_key(InputEvent *keyinput);
static void ELCALLBACK   PsychEyelink_alert_printf_hook(const char *msg);

// Defined in EyelinkSynopsis.c
void		InitializeSynopsis();
PsychError	PsychDisplayEyelinkSynopsis(void);

// Defined in MODULEVersion.c
PsychError	MODULEVersion(void);

// Defined in EyelinkShutdown.c
PsychError PsychEyelinkShutdown(void);

// Eyelink Target sub-commands
PsychError EyelinkButtonStates(void);
PsychError EyelinkCommand(void);
PsychError EyelinkCurrentMode(void);
PsychError EyelinkImageModeDisplay(void);
PsychError EyelinkInitialize(void);
PsychError EyelinkIsConnected(void);
PsychError EyelinkNewestFloatSample(void);
PsychError EyelinkSendKeyButton(void);
PsychError EyelinkShutdown(void);
PsychError EyelinkStartRecording(void);
PsychError EyelinkStartSetup(void);
PsychError EyelinkStopRecording(void);
PsychError EyelinkWaitForModeReady(void);
PsychError EyelinkTargetCheck(void);
PsychError EyelinkAcceptTrigger(void);
PsychError EyelinkCalResult(void);
PsychError EyelinkMessage(void);
PsychError EyelinkOpenFile(void);
PsychError EyelinkCloseFile(void);
PsychError EyelinkCheckRecording(void);
PsychError EyelinkNewFloatSampleAvailable(void);
PsychError EyelinkSetOfflineMode(void);
PsychError EyelinkDriftCorrStart(void);
PsychError EyelinkApplyDriftCorr(void);
PsychError EyelinkEyeAvailable(void);
PsychError EyelinkInitializeDummy(void);
PsychError EyelinkReadTime(void);
PsychError EyelinkRequestTime(void);
PsychError EyelinkReceiveFile(void);
PsychError EyelinkGetTrackerVersion(void);

PsychError EyelinkNewestFloatSampleRaw(void);
PsychError EyelinkGetNextDataType(void);
PsychError EyelinkGetFloatData(void);
PsychError EyelinkGetFloatDataRaw(void);
PsychError EyelinkGetQueuedItems(void);

PsychError EyelinkTrackerTime(void);
PsychError EyelinkTimeOffset(void);
PsychError EyelinkVerbosity(void);

// PSYCH_IS_INCLUDED_Eyelink
#endif
