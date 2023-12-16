/*
    PsychSourceGL/Source/Common/Eyelink/PsychEyelink.h

    PROJECTS: Eyelink

    AUTHORS:

    cburns@berkeley.edu                 cdb
    E.Peters@ai.rug.nl                  emp
    f.w.cornelissen@med.rug.nl          fwc

    PLATFORMS:    All.

    HISTORY:

    11/21/05  cdb        Created.

*/

#ifndef PSYCH_IS_INCLUDED_Eyelink
#define PSYCH_IS_INCLUDED_Eyelink

// PsychToolbox Includes
#include "Psych.h"

// Eyelink Includes
#include "eyelink.h"
#include "core_expt.h"

/////////////////////////////////////////////////////////////////////////
// Global variables used throughout eyelink C files

extern int        giSystemInitialized;

/////////////////////////////////////////////////////////////////////////
//        Eyelink Function prototypes

// Defined in EyelinkCreateDataStructs.c
mxArray *CreateMXFSample(const FSAMPLE *fs);
mxArray *CreateMXFSampleRaw(const FSAMPLE_RAW *fs);
mxArray *CreateMXISample(const ISAMPLE *is);
mxArray *CreateMXFEvent(const FEVENT *fe);
mxArray *CreateMXIEvent(const IEVENT *ie);

// Defined in PsychEyelink.c
int Verbosity(void);
const char* PsychEyelinkParseToString(int startIdx);
PsychError EyelinkSystemIsConnected(void);
PsychError EyelinkSystemIsInitialized(void);
void PsychEyelink_init_core_graphics(const char* callback);
void PsychEyelink_uninit_core_graphics(void);
void PsychEyelink_TestEyeImage(void);
void PsychEyelink_dumpHookfunctions(void);

// Defined in EyelinkSynopsis.c
void InitializeSynopsis();
PsychError PsychDisplayEyelinkSynopsis(void);

// Defined in MODULEVersion.c
PsychError MODULEVersion(void);

// Defined in EyelinkShutdown.c
PsychError PsychEyelinkShutdown(void);

// Helpers
psych_bool TrackerOKForRawValues(void); //defined in EyelinkGetQueuedData.c

// Eyelink Target sub-commands
PsychError EyelinkButtonStates(void);
PsychError EyelinkCommand(void);
PsychError EyelinkCurrentMode(void);
PsychError EyelinkImageModeDisplay(void);
PsychError EyelinkInitialize(void);
PsychError EyelinkIsConnected(void);
PsychError EyelinkSetAddress(void);
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
PsychError EyelinkGetQueuedData(void);

PsychError EyelinkTrackerTime(void);
PsychError EyelinkTimeOffset(void);
PsychError EyelinkVerbosity(void);
PsychError EyelinkTestSuite(void);

//NJ
PsychError EyelinkImageTransfer(void);
PsychError EyelinkTrackerMode(void);
PsychError EyelinkCalMessage(void);
PsychError EyelinkReadFromTracker(void);

//BR
PsychError EyelinkEDKVersion(void);
PsychError EyelinkSendFile(void);

// PSYCH_IS_INCLUDED_Eyelink
#endif
