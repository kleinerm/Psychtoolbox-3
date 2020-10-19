/*
 * PsychToolbox/Source/Common/PsychVulkanCore/PsychVulkan.h
 *
 * PROJECTS: PsychVulkanCore only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   Linux, Windows.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for interfacing with the Vulkan graphics rendering API
 * for special purpose display and compute tasks.
 *
 * Copyright (c) 2020 Mario Kleiner. Licensed under the MIT license:
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
#ifndef PSYCH_IS_INCLUDED_PsychVulkan
#define PSYCH_IS_INCLUDED_PsychVulkan

// project includes
#include "Psych.h"

#ifdef  __cplusplus
extern "C" {
#endif

// function prototypes
PsychError MODULEVersion(void);

void InitializeSynopsis(void);
PsychError PsychVulkanDisplaySynopsis(void);

void PsychVulkanInit(void);
PsychError PsychVulkanShutDown(void);

PsychError PSYCHVULKANVerbosity(void);
PsychError PSYCHVULKANGetCount(void);
PsychError PSYCHVULKANGetDevices(void);
PsychError PSYCHVULKANClose(void);
PsychError PSYCHVULKANOpenWindow(void);
PsychError PSYCHVULKANCloseWindow(void);
PsychError PSYCHVULKANGetHDRProperties(void);
PsychError PSYCHVULKANHDRLocalDimming(void);
PsychError PSYCHVULKANHDRMetadata(void);
PsychError PSYCHVULKANPresent(void);
PsychError PSYCHVULKANGetInteropHandle(void);

#ifdef  __cplusplus
}
#endif

// end include once
#endif
