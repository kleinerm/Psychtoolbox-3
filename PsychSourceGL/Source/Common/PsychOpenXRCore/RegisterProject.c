/*
 * Psychtoolbox-3/PsychSourceGL/Source/Common/PsychOpenXRCore/RegisterProject.c
 *
 * PROJECTS: PsychOpenXRCore only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   All which are supported by an OpenXR Runtime.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for virtual reality (VR), augmented reality (AR) and
 * mixed reality (MR) devices, also known as XR devices, which are supported by
 * an OpenXR-1.0 (or later versions) compliant OpenXR runtime.
 * The runtime must support OpenGL as rendering backend for XR content.
 *
 * The driver was initially derived from PsychOculusVRCore1.
 * Copyright (c) 2022-2023 Mario Kleiner. Licensed to you under the MIT license:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

//begin include once
#include "Psych.h"
#include "PsychOpenXRCore.h"

PsychError PsychModuleInit(void)
{
    // Register the project exit function:
    PsychErrorExit(PsychRegisterExit(&PsychOpenXRCoreShutDown));

    // Register the project function which is called when the module
    // is invoked with no named subfunction:
    PsychErrorExit(PsychRegister(NULL, &PsychOpenXRCoreDisplaySynopsis));

    // Register user-callable subfunctions:
    PsychErrorExit(PsychRegister("Open", &PSYCHOPENXROpen));
    PsychErrorExit(PsychRegister("Close", &PSYCHOPENXRClose));
    PsychErrorExit(PsychRegister("Start", &PSYCHOPENXRStart));
    PsychErrorExit(PsychRegister("Stop", &PSYCHOPENXRStop));
    PsychErrorExit(PsychRegister("GetCount", &PSYCHOPENXRGetCount));
    PsychErrorExit(PsychRegister("Controllers", &PSYCHOPENXRControllers));
    PsychErrorExit(PsychRegister("GetTrackingState", &PSYCHOPENXRGetTrackingState));
    PsychErrorExit(PsychRegister("GetInputState", &PSYCHOPENXRGetInputState));
    PsychErrorExit(PsychRegister("GetFovTextureSize", &PSYCHOPENXRGetFovTextureSize));
    PsychErrorExit(PsychRegister("GetStaticRenderParameters", &PSYCHOPENXRGetStaticRenderParameters));
    PsychErrorExit(PsychRegister("PresentFrame", &PSYCHOPENXRPresentFrame));
    PsychErrorExit(PsychRegister("Verbosity", &PSYCHOPENXRVerbosity));
    PsychErrorExit(PsychRegister("ReferenceSpaceType", &PSYCHOPENXRReferenceSpaceType));
    PsychErrorExit(PsychRegister("HapticPulse", &PSYCHOPENXRHapticPulse));
    PsychErrorExit(PsychRegister("CreateRenderTextureChain", &PSYCHOPENXRCreateRenderTextureChain));
    PsychErrorExit(PsychRegister("GetNextTextureHandle", &PSYCHOPENXRGetNextTextureHandle));
    PsychErrorExit(PsychRegister("CreateAndStartSession", &PSYCHOPENXRCreateAndStartSession));
    PsychErrorExit(PsychRegister("ViewType", &PSYCHOPENXRViewType));
    PsychErrorExit(PsychRegister("View2DParameters", &PSYCHOPENXRView2DParameters));
    PsychErrorExit(PsychRegister("NeedLocateForProjectionLayers", &PSYCHOPENXRNeedLocateForProjectionLayers));
    PsychErrorExit(PsychRegister("PresenterThreadEnable", &PSYCHOPENXRPresenterThreadEnable));
    PsychErrorExit(PsychRegister("TimingSupport", &PSYCHOPENXRTimingSupport));
    //PsychErrorExit(PsychRegister("GetEyePose", &PSYCHOPENXRGetEyePose));
    //PsychErrorExit(PsychRegister("GetTrackersState", &PSYCHOPENXRGetTrackersState));
    //PsychErrorExit(PsychRegister("", &PSYCHOPENXR));
    //PsychErrorExit(PsychRegister("", &PSYCHOPENXR));
    //PsychErrorExit(PsychRegister("", &PSYCHOPENXR));

    // Report the version
    PsychErrorExit(PsychRegister("Version", &MODULEVersion));

    // Register the module name & authors:
    PsychErrorExit(PsychRegister("PsychOpenXRCore", NULL));
    PsychSetModuleAuthorByInitials("mk");

    // Register synopsis and named subfunctions.
    InitializeSynopsis();

    // Preinit everything:
    PsychOpenXRCoreInit();

    return(PsychError_none);
}
