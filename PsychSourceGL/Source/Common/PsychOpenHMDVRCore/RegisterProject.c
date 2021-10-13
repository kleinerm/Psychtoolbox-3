/*
 * PsychToolbox/Source/Common/PsychOpenHMDVRCore/RegisterProject.c
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

//begin include once
#include "Psych.h"
#include "PsychOpenHMDVRCore.h"

PsychError PsychModuleInit(void)
{
    // Register the project exit function
    PsychErrorExit(PsychRegisterExit(&PsychOpenHMDVRShutDown));

    // Register the project function which is called when the module
    // is invoked with no named subfunction:
    PsychErrorExit(PsychRegister(NULL,  &PsychOpenHMDVRDisplaySynopsis));

    PsychErrorExit(PsychRegister("Open", &PSYCHOPENHMDVROpen));
    PsychErrorExit(PsychRegister("Close", &PSYCHOPENHMDVRClose));
    PsychErrorExit(PsychRegister("Start", &PSYCHOPENHMDVRStart));
    PsychErrorExit(PsychRegister("Stop", &PSYCHOPENHMDVRStop));
    PsychErrorExit(PsychRegister("GetCount", &PSYCHOPENHMDVRGetCount));
    PsychErrorExit(PsychRegister("GetTrackingState", &PSYCHOPENHMDVRGetTrackingState));
    PsychErrorExit(PsychRegister("GetFovTextureSize", &PSYCHOPENHMDVRGetFovTextureSize));
    PsychErrorExit(PsychRegister("GetUndistortionParameters", &PSYCHOPENHMDVRGetUndistortionParameters));
    PsychErrorExit(PsychRegister("GetCorrectionShaders", &PSYCHOPENHMDVRGetCorrectionShaders));
    PsychErrorExit(PsychRegister("GetStaticRenderParameters", &PSYCHOPENHMDVRGetStaticRenderParameters));
    PsychErrorExit(PsychRegister("Verbosity", &PSYCHOPENHMDVRVerbosity));
    PsychErrorExit(PsychRegister("GetEyePose", &PSYCHOPENHMDVRGetEyePose));
    PsychErrorExit(PsychRegister("SetLowPersistence", &PSYCHOPENHMDVRSetLowPersistence));
    PsychErrorExit(PsychRegister("SetDynamicPrediction", &PSYCHOPENHMDVRSetDynamicPrediction));
    PsychErrorExit(PsychRegister("GetInputState", &PSYCHOPENHMDVRGetInputState));
    PsychErrorExit(PsychRegister("HapticPulse", &PSYCHOPENHMDVRHapticPulse));

    //PsychErrorExit(PsychRegister("", &PSYCHOPENHMDVR));

    // Report the version
    PsychErrorExit(PsychRegister("Version", &MODULEVersion));

    // Register the module name & authors:
    PsychErrorExit(PsychRegister("PsychOpenHMDVRCore", NULL));
    PsychSetModuleAuthorByInitials("mk");

    // Register synopsis and named subfunctions.
    InitializeSynopsis();

    // Preinit everything:
    PsychOpenHMDVRInit();

    return(PsychError_none);
}
