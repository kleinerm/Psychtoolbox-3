/*
 * PsychToolbox/Source/Common/PsychOculusVRCore/RegisterProject.c
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

//begin include once
#include "Psych.h"
#include "PsychOculusVR.h"

PsychError PsychModuleInit(void)
{
    // Register the project exit function
    PsychErrorExit(PsychRegisterExit(&PsychOculusVRShutDown));

    // Register the project function which is called when the module
    // is invoked with no named subfunction:
    PsychErrorExit(PsychRegister(NULL,  &PsychOculusVRDisplaySynopsis));

    PsychErrorExit(PsychRegister("Open", &PSYCHOCULUSVROpen));
    PsychErrorExit(PsychRegister("Close", &PSYCHOCULUSVRClose));
    PsychErrorExit(PsychRegister("Start", &PSYCHOCULUSVRStart));
    PsychErrorExit(PsychRegister("Stop", &PSYCHOCULUSVRStop));
    PsychErrorExit(PsychRegister("GetCount", &PSYCHOCULUSVRGetCount));
    PsychErrorExit(PsychRegister("GetTrackingState", &PSYCHOCULUSVRGetTrackingState));
    PsychErrorExit(PsychRegister("GetFovTextureSize", &PSYCHOCULUSVRGetFovTextureSize));
    PsychErrorExit(PsychRegister("GetUndistortionParameters", &PSYCHOCULUSVRGetUndistortionParameters));
    PsychErrorExit(PsychRegister("GetEyeTimewarpMatrices", &PSYCHOCULUSVRGetEyeTimewarpMatrices));
    PsychErrorExit(PsychRegister("StartRender", &PSYCHOCULUSVRStartRender));
    PsychErrorExit(PsychRegister("GetStaticRenderParameters", &PSYCHOCULUSVRGetStaticRenderParameters));
    PsychErrorExit(PsychRegister("EndFrameTiming", &PSYCHOCULUSVREndFrameTiming));
    PsychErrorExit(PsychRegister("Verbosity", &PSYCHOCULUSVRVerbosity));
    PsychErrorExit(PsychRegister("GetEyePose", &PSYCHOCULUSVRGetEyePose));
    PsychErrorExit(PsychRegister("GetHSWState", &PSYCHOCULUSVRGetHSWState));
    PsychErrorExit(PsychRegister("SetLowPersistence", &PSYCHOCULUSVRSetLowPersistence));
    PsychErrorExit(PsychRegister("SetDynamicPrediction", &PSYCHOCULUSVRSetDynamicPrediction));
    PsychErrorExit(PsychRegister("LatencyTester", &PSYCHOCULUSVRLatencyTester));

    //PsychErrorExit(PsychRegister("", &PSYCHOCULUSVR));
    //PsychErrorExit(PsychRegister("", &PSYCHOCULUSVR));
    //PsychErrorExit(PsychRegister("", &PSYCHOCULUSVR));

    // Report the version
    PsychErrorExit(PsychRegister("Version", &MODULEVersion));

    // Register the module name & authors:
    PsychErrorExit(PsychRegister("PsychOculusVRCore", NULL));
    PsychSetModuleAuthorByInitials("mk");

    // Register synopsis and named subfunctions.
    InitializeSynopsis();

    // Preinit everything:
    PsychOculusVRInit();

    return(PsychError_none);
}
