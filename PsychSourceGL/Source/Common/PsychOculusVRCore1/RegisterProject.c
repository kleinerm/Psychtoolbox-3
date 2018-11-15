/*
 * PsychToolbox/Source/Common/PsychOculusVRCore1/RegisterProject.c
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
 * head sets, using the OculusVR 1.0 SDK and runtime and later.
 *
 */

//begin include once
#include "Psych.h"
#include "PsychOculusVR1.h"

PsychError PsychModuleInit(void)
{
    // Register the project exit function
    PsychErrorExit(PsychRegisterExit(&PsychOculusVR1ShutDown));

    // Register the project function which is called when the module
    // is invoked with no named subfunction:
    PsychErrorExit(PsychRegister(NULL,  &PsychOculusVR1DisplaySynopsis));

    PsychErrorExit(PsychRegister("Open", &PSYCHOCULUSVR1Open));
    PsychErrorExit(PsychRegister("Close", &PSYCHOCULUSVR1Close));
    PsychErrorExit(PsychRegister("Start", &PSYCHOCULUSVR1Start));
    PsychErrorExit(PsychRegister("Stop", &PSYCHOCULUSVR1Stop));
    PsychErrorExit(PsychRegister("GetCount", &PSYCHOCULUSVR1GetCount));
    PsychErrorExit(PsychRegister("GetTrackingState", &PSYCHOCULUSVR1GetTrackingState));
    PsychErrorExit(PsychRegister("GetFovTextureSize", &PSYCHOCULUSVR1GetFovTextureSize));
    PsychErrorExit(PsychRegister("GetUndistortionParameters", &PSYCHOCULUSVR1GetUndistortionParameters));
    PsychErrorExit(PsychRegister("GetTrackersState", &PSYCHOCULUSVR1GetTrackersState));
    PsychErrorExit(PsychRegister("StartRender", &PSYCHOCULUSVR1StartRender));
    PsychErrorExit(PsychRegister("GetStaticRenderParameters", &PSYCHOCULUSVR1GetStaticRenderParameters));
    PsychErrorExit(PsychRegister("EndFrameRender", &PSYCHOCULUSVR1EndFrameRender));
    PsychErrorExit(PsychRegister("PresentFrame", &PSYCHOCULUSVR1PresentFrame));
    PsychErrorExit(PsychRegister("Verbosity", &PSYCHOCULUSVR1Verbosity));
    PsychErrorExit(PsychRegister("GetEyePose", &PSYCHOCULUSVR1GetEyePose));
    PsychErrorExit(PsychRegister("SetHUDState", &PSYCHOCULUSVR1SetHUDState));
    PsychErrorExit(PsychRegister("VRAreaBoundary", &PSYCHOCULUSVR1VRAreaBoundary));
    PsychErrorExit(PsychRegister("SetDynamicPrediction", &PSYCHOCULUSVR1SetDynamicPrediction));
    PsychErrorExit(PsychRegister("GetPerformanceStats", &PSYCHOCULUSVR1GetPerformanceStats));

    PsychErrorExit(PsychRegister("CreateRenderTextureChain", &PSYCHOCULUSVR1CreateRenderTextureChain));
    PsychErrorExit(PsychRegister("GetNextTextureHandle", &PSYCHOCULUSVR1GetNextTextureHandle));
    PsychErrorExit(PsychRegister("CreateMirrorTexture", &PSYCHOCULUSVR1CreateMirrorTexture));
    //PsychErrorExit(PsychRegister("", &PSYCHOCULUSVR1));

    // Report the version
    PsychErrorExit(PsychRegister("Version", &MODULEVersion));

    // Register the module name & authors:
    PsychErrorExit(PsychRegister("PsychOculusVRCore1", NULL));
    PsychSetModuleAuthorByInitials("mk");

    // Register synopsis and named subfunctions.
    InitializeSynopsis();

    // Preinit everything:
    PsychOculusVR1Init();

    return(PsychError_none);
}
