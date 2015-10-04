/*
 * PsychToolbox/Source/Common/PsychOculusVRCore/PsychOculusVR.h
 *
 * PROJECTS: PsychOculusVRCore only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   All.
 *
 * HISTORY:
 *
 * 1.09.2015   mk      Created.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for the Oculus VR virtual reality
 * head sets. The initial version will support the Rift DK2,
 * and possibly the old Rift DK1, although that hasn't been
 * tested.
 *
 */

// begin include once
#ifndef PSYCH_IS_INCLUDED_PsychOculusVR
#define PSYCH_IS_INCLUDED_PsychOculusVR

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"

#ifdef  __cplusplus
extern "C" {
#endif

//function prototypes
PsychError MODULEVersion(void);
void InitializeSynopsis(void);
PsychError PsychOculusVRDisplaySynopsis(void);

void PsychOculusVRInit(void);
PsychError PsychOculusVRShutDown(void);

PsychError PSYCHOCULUSVRVerbosity(void);
PsychError PSYCHOCULUSVROpen(void);
PsychError PSYCHOCULUSVRClose(void);
PsychError PSYCHOCULUSVRGetHSWState(void);
PsychError PSYCHOCULUSVRStart(void);
PsychError PSYCHOCULUSVRStop(void);
PsychError PSYCHOCULUSVRGetCount(void);
PsychError PSYCHOCULUSVRGetTrackingState(void);
PsychError PSYCHOCULUSVRGetFovTextureSize(void);
PsychError PSYCHOCULUSVRGetUndistortionParameters(void);
PsychError PSYCHOCULUSVRGetStaticRenderParameters(void);
PsychError PSYCHOCULUSVRGetEyeTimewarpMatrices(void);
PsychError PSYCHOCULUSVRStartRender(void);
PsychError PSYCHOCULUSVREndFrameTiming(void);
PsychError PSYCHOCULUSVRGetEyePose(void);
PsychError PSYCHOCULUSVRSetLowPersistence(void);
PsychError PSYCHOCULUSVRSetDynamicPrediction(void);
PsychError PSYCHOCULUSVRLatencyTester(void);
    
#ifdef  __cplusplus
}
#endif

//end include once
#endif
