/*
 * PsychToolbox/Source/Common/PsychOpenHMDVRCore/PsychOpenHMDVRCore.h
 *
 * PROJECTS: PsychOpenHMDVRCore only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   All.
 *
 * HISTORY:
 *
 * 30.07.2017   mk      Created.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for the virtual reality head sets
 * supported via OpenHMD.
 */

// begin include once
#ifndef PSYCH_IS_INCLUDED_PsychOpenHMDVRCore
#define PSYCH_IS_INCLUDED_PsychOpenHMDVRCore

//project includes
#include "Psych.h"
#include "PsychTimeGlue.h"

#ifdef  __cplusplus
extern "C" {
#endif

//function prototypes
PsychError MODULEVersion(void);
void InitializeSynopsis(void);
PsychError PsychOpenHMDVRDisplaySynopsis(void);

void PsychOpenHMDVRInit(void);
PsychError PsychOpenHMDVRShutDown(void);

PsychError PSYCHOPENHMDVRVerbosity(void);
PsychError PSYCHOPENHMDVROpen(void);
PsychError PSYCHOPENHMDVRClose(void);
PsychError PSYCHOPENHMDVRStart(void);
PsychError PSYCHOPENHMDVRStop(void);
PsychError PSYCHOPENHMDVRGetCount(void);
PsychError PSYCHOPENHMDVRGetTrackingState(void);
PsychError PSYCHOPENHMDVRGetFovTextureSize(void);
PsychError PSYCHOPENHMDVRGetUndistortionParameters(void);
PsychError PSYCHOPENHMDVRGetStaticRenderParameters(void);
PsychError PSYCHOPENHMDVRGetCorrectionShaders(void);
PsychError PSYCHOPENHMDVRGetEyePose(void);
PsychError PSYCHOPENHMDVRSetLowPersistence(void);
PsychError PSYCHOPENHMDVRSetDynamicPrediction(void);

#ifdef  __cplusplus
}
#endif

//end include once
#endif
