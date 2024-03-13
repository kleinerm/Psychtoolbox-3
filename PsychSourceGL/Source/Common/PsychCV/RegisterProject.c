/*
 *    PsychToolbox-3/PsychSourceGL/Source/Common/PsychCV/RegisterProject.c
 *
 *    PLATFORMS:  All
 *
 *    AUTHORS:
 *
 *    Mario Kleiner   mk  mario.kleiner.de@gmail.com
 *
 *    HISTORY:
 *
 *    5.01.2008       mk  wrote it.
 *
 */

#include "Psych.h"
#include "PsychCV.h"

PsychError PsychModuleInit(void)
{
    // Register the project exit function
    PsychErrorExit(PsychRegisterExit(&PsychCVExit));

    // Register the project function which is called when the module
    // is invoked with no arguments:
    PsychErrorExit(PsychRegister(NULL, &PSYCHCVDisplaySynopsis));

    // Report the version
    PsychErrorExit(PsychRegister("Version", &MODULEVersion));

    // Register the module name
    PsychErrorExit(PsychRegister("PsychCV", NULL));

    // Register synopsis and named subfunctions.
    PsychErrorExit(PsychRegister("Verbosity", &PSYCHCVVerbosity));
    PsychErrorExit(PsychRegister("CopyMatrixToMemBuffer", &PSYCHCVCopyMatrixToMemBuffer));

    #ifdef PSYCHCV_USE_OPENCV
    PsychErrorExit(PsychRegister("OpenEyesInitialize", &PSYCHCVOpenEyesInitialize));
    PsychErrorExit(PsychRegister("OpenEyesShutdown", &PSYCHCVOpenEyesShutdown));
    PsychErrorExit(PsychRegister("OpenEyesParameters", &PSYCHCVOpenEyesParameters));
    PsychErrorExit(PsychRegister("OpenEyesTrackEyePosition", &PSYCHCVOpenEyesTrackEyePosition));
    #endif

    #ifdef PSYCHCV_USE_ARTOOLKIT
    PsychErrorExit(PsychRegister("ARInitialize", &PSYCHCVARInitialize));
    PsychErrorExit(PsychRegister("ARShutdown", &PSYCHCVARShutdown));
    PsychErrorExit(PsychRegister("ARLoadMarker", &PSYCHCVARLoadMarker));
    PsychErrorExit(PsychRegister("ARDetectMarkers", &PSYCHCVARDetectMarkers));
    PsychErrorExit(PsychRegister("ARRenderImage", &PSYCHCVARRenderImage));
    PsychErrorExit(PsychRegister("ARTrackerSettings", &PSYCHCVARTrackerSettings));
    PsychErrorExit(PsychRegister("ARRenderSettings", &PSYCHCVARRenderSettings));
    #endif

    #ifdef PSYCHCV_USE_APRILTAGS
    PsychErrorExit(PsychRegister("AprilInitialize", &PSYCHCVAprilInitialize));
    PsychErrorExit(PsychRegister("AprilShutdown", &PSYCHCVAprilShutdown));
    PsychErrorExit(PsychRegister("AprilDetectMarkers", &PSYCHCVAprilDetectMarkers));
    PsychErrorExit(PsychRegister("AprilSettings", &PSYCHCVAprilSettings));
    PsychErrorExit(PsychRegister("April3DSettings", &PSYCHCVApril3DSettings));
    #endif

    // Setup synopsis help strings:
    InitializeSynopsis();

    // Setup module author:
    PsychSetModuleAuthorByInitials("mk");

    // Call wait-routine for 0.1 secs: This to initialize the time glue on MS-Windows,
    // so the first call to a timing function won't delay:
    PsychWaitIntervalSeconds(0.1);

    // Perform all remaining initialization:
    PsychCVInitialize();

    // Startup finished.
    return(PsychError_none);
}
