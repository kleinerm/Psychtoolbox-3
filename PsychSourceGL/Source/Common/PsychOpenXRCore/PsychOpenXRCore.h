/*
 * Psychtoolbox-3/PsychSourceGL/Source/Common/PsychOpenXRCore/PsychOpenXRCore.h
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

// begin include once
#ifndef PSYCH_IS_INCLUDED_PsychOpenXRCore
#define PSYCH_IS_INCLUDED_PsychOpenXRCore

// Project includes:
#include "Psych.h"

#ifdef __cplusplus
extern "C" {
#endif

// Function prototypes:
PsychError MODULEVersion(void);
void InitializeSynopsis(void);
PsychError PsychOpenXRCoreDisplaySynopsis(void);
void PsychOpenXRCoreInit(void);
PsychError PsychOpenXRCoreShutDown(void);

PsychError PSYCHOPENXRVerbosity(void);
PsychError PSYCHOPENXROpen(void);
PsychError PSYCHOPENXRClose(void);
PsychError PSYCHOPENXRCreateAndStartSession(void);
PsychError PSYCHOPENXRStart(void);
PsychError PSYCHOPENXRStop(void);
PsychError PSYCHOPENXRGetCount(void);
PsychError PSYCHOPENXRGetTrackingState(void);
PsychError PSYCHOPENXRControllers(void);
PsychError PSYCHOPENXRGetInputState(void);
PsychError PSYCHOPENXRGetFovTextureSize(void);
PsychError PSYCHOPENXRCreateRenderTextureChain(void);
PsychError PSYCHOPENXRGetNextTextureHandle(void);
PsychError PSYCHOPENXRGetStaticRenderParameters(void);
PsychError PSYCHOPENXRGetTrackersState(void);
PsychError PSYCHOPENXRPresentFrame(void);
PsychError PSYCHOPENXRReferenceSpaceType(void);
PsychError PSYCHOPENXRHapticPulse(void);
PsychError PSYCHOPENXRGetEyePose(void);
PsychError PSYCHOPENXRViewType(void);
PsychError PSYCHOPENXRView2DParameters(void);
PsychError PSYCHOPENXRNeedLocateForProjectionLayers(void);
PsychError PSYCHOPENXRPresenterThreadEnable(void);
PsychError PSYCHOPENXRTimingSupport(void);

#ifdef __cplusplus
}
#endif

// end include once
#endif
