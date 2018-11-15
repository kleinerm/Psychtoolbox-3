/*
 * PsychToolbox/Source/Common/PsychOculusVRCore1/PsychOculusVR1.h
 *
 * PROJECTS: PsychOculusVRCore1 only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   All.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for the Oculus VR virtual reality
 * head sets, using the OculusVR 1.0 SDK and runtime.
 *
 */

// begin include once
#ifndef PSYCH_IS_INCLUDED_PsychOculusVR1
#define PSYCH_IS_INCLUDED_PsychOculusVR1

//project includes
#include "Psych.h"

#ifdef  __cplusplus
extern "C" {
#endif

//function prototypes
PsychError MODULEVersion(void);
void InitializeSynopsis(void);
PsychError PsychOculusVR1DisplaySynopsis(void);

void PsychOculusVR1Init(void);
PsychError PsychOculusVR1ShutDown(void);

PsychError PSYCHOCULUSVR1Verbosity(void);
PsychError PSYCHOCULUSVR1Open(void);
PsychError PSYCHOCULUSVR1Close(void);
PsychError PSYCHOCULUSVR1GetHSWState(void);
PsychError PSYCHOCULUSVR1Start(void);
PsychError PSYCHOCULUSVR1Stop(void);
PsychError PSYCHOCULUSVR1GetCount(void);
PsychError PSYCHOCULUSVR1GetTrackingState(void);
PsychError PSYCHOCULUSVR1GetFovTextureSize(void);
PsychError PSYCHOCULUSVR1CreateRenderTextureChain(void);
PsychError PSYCHOCULUSVR1GetNextTextureHandle(void);
PsychError PSYCHOCULUSVR1CreateMirrorTexture(void);
PsychError PSYCHOCULUSVR1GetUndistortionParameters(void);
PsychError PSYCHOCULUSVR1GetStaticRenderParameters(void);
PsychError PSYCHOCULUSVR1GetEyeTimewarpMatrices(void);
PsychError PSYCHOCULUSVR1StartRender(void);
PsychError PSYCHOCULUSVR1EndFrameRender(void);
PsychError PSYCHOCULUSVR1PresentFrame(void);
PsychError PSYCHOCULUSVR1GetEyePose(void);
PsychError PSYCHOCULUSVR1SetLowPersistence(void);
PsychError PSYCHOCULUSVR1SetDynamicPrediction(void);
PsychError PSYCHOCULUSVR1LatencyTester(void);

#ifdef  __cplusplus
}
#endif

//end include once
#endif
