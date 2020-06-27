/*
 * PsychToolbox/Source/Common/PsychVulkanCore/RegisterProject.c
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

//begin include once
#include "Psych.h"
#include "PsychVulkan.h"

PsychError PsychModuleInit(void)
{
    // Register the project exit function:
    PsychErrorExit(PsychRegisterExit(&PsychVulkanShutDown));

    // Register the project function which is called when the module
    // is invoked with no named subfunction:
    PsychErrorExit(PsychRegister(NULL,  &PsychVulkanDisplaySynopsis));

    PsychErrorExit(PsychRegister("Verbosity", &PSYCHVULKANVerbosity));
    PsychErrorExit(PsychRegister("GetCount", &PSYCHVULKANGetCount));
    PsychErrorExit(PsychRegister("Close", &PSYCHVULKANClose));
    PsychErrorExit(PsychRegister("Present", &PSYCHVULKANPresent));
    PsychErrorExit(PsychRegister("OpenWindow", &PSYCHVULKANOpenWindow));
    PsychErrorExit(PsychRegister("CloseWindow", &PSYCHVULKANCloseWindow));
    PsychErrorExit(PsychRegister("GetInteropHandle", &PSYCHVULKANGetInteropHandle));
    PsychErrorExit(PsychRegister("GetDevices", &PSYCHVULKANGetDevices));
    PsychErrorExit(PsychRegister("GetHDRProperties", &PSYCHVULKANGetHDRProperties));
    PsychErrorExit(PsychRegister("HDRMetadata", &PSYCHVULKANHDRMetadata));
    PsychErrorExit(PsychRegister("HDRLocalDimming", &PSYCHVULKANHDRLocalDimming));
    //PsychErrorExit(PsychRegister("", &PSYCHVULKAN));
    //PsychErrorExit(PsychRegister("", &PSYCHVULKAN));
    //PsychErrorExit(PsychRegister("", &PSYCHVULKAN));

    // Report the version:
    PsychErrorExit(PsychRegister("Version", &MODULEVersion));

    // Register the module name and authors:
    PsychErrorExit(PsychRegister("PsychVulkanCore", NULL));
    PsychSetModuleAuthorByInitials("mk");

    // Register synopsis and named subfunctions.
    InitializeSynopsis();

    // Preinit everything:
    PsychVulkanInit();

    return(PsychError_none);
}
