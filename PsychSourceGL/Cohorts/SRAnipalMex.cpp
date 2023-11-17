/*
 * SRAnipalMex - A mex file for using the HTC SRAnipal runtime to access the eye gaze
 * trackers of various HTC Vive Virtual Reality head mounted displays (VR HMDs) under
 * Microsoft Windows, to get more detailed gaze information than with standard OpenXR
 * eye tracking extensions. Developed and tested on Microsoft Windows 10 22H2 64-Bit
 * with SteamVR 1.25.6, SRAnipal SDK v1.3.6.8, under Matlab R2022b and GNU/Octave 7.3
 * on a "HTC Vive Pro Eye" VR HMD with builtin Tobii binocular 120 Hz eyetracker.
 *
 * Most code is:
 *
 * Copyright (c) 2023 Mario Kleiner. Licensed to you under MIT license.
 *
 * Small bits of this mex files source code were derived from the SRAnipal sample code
 * in the SDK, specifically the trivial error code mapping to a string.
 *
 * Compiling this mex file under Matlab or Octave requires a properly installed
 * SRAnipalCSDK in a folder side-by-side to the Psychtoolbox-3 root folder. Our standard
 * PTB make script windowsmakeit64_twisty builds it via windowsmakeit64_twisty(5);
 *
 * A minimal alternative build sequence looks like this, with pathes adjusted accordingly:
 * Matlab: mex -output bin\SRAnipalMex -I.\include SRAnipalMex.cpp -L.\lib -lSRanipal
 * Octave: mex --output bin\SRAnipalMex -I.\include SRAnipalMex.cpp -L.\lib -lSRanipal
 *
 * The SDK can be downloaded - after registering a free developer account and agreeing
 * to a license agreement - from HTC's developer website under:
 * https://developer-express.vive.com/resources/vive-sense/eye-and-facial-tracking-sdk/
 *
 * Specific SDK installation instructions for building the mex file can be found inside
 * the windowsmakeit64_twisty file in the relevant build section for the Matlab mex file.
 *
 * The SDK used for building this mex file has the following license (from the "SRanipal.h"
 * main SDK header file), which we include here literally, as follows:
 */

///////////////////////////////////////////////////////////////////////////////////////
//
//                           License Agreement
//                     For Vive Super Reality Library
//
// Copyright (c) 2017,		HTC Corporation
//
// All rights reserved. Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
///////////////////////////////////////////////////////////////////////////////////////


/* Standard includes for Windows and math functions: */
#include <windows.h>
#include <math.h>

/* Matlab/Octave includes: */
#include "mex.h"

/* HTC SRAnipal includes: */
#include "SRanipal.h"
#include "SRanipal_Eye.h"

using namespace ViveSR;

static unsigned int initialized = 0;

#define MAX_SAMPLES 12000
CRITICAL_SECTION mutex;
unsigned int readidx, writeidx;
ViveSR::anipal::Eye::EyeData_v2 eyeSamples[MAX_SAMPLES];
bool doesNeedCalibration;

const char* ErrorToString(int error)
{
    static char result[64] = { 0 };

    switch (error) {
        case(RUNTIME_NOT_FOUND):     sprintf(result, "RUNTIME_NOT_FOUND"); break;
        case(NOT_INITIAL):           sprintf(result, "NOT_INITIAL"); break;
        case(FAILED):                sprintf(result, "FAILED"); break;
        case(WORK):                  sprintf(result, "WORK"); break;
        case(INVALID_INPUT):         sprintf(result, "INVALID_INPUT"); break;
        case(FILE_NOT_FOUND):        sprintf(result, "FILE_NOT_FOUND"); break;
        case(DATA_NOT_FOUND):        sprintf(result, "DATA_NOT_FOUND"); break;
        case(UNDEFINED):             sprintf(result, "UNDEFINED"); break;
        case(INITIAL_FAILED):        sprintf(result, "INITIAL_FAILED"); break;
        case(NOT_IMPLEMENTED):       sprintf(result, "NOT_IMPLEMENTED"); break;
        case(NULL_POINTER):          sprintf(result, "NULL_POINTER"); break;
        case(OVER_MAX_LENGTH):       sprintf(result, "OVER_MAX_LENGTH"); break;
        case(FILE_INVALID):          sprintf(result, "FILE_INVALID"); break;
        case(UNINSTALL_STEAM):       sprintf(result, "UNINSTALL_STEAM"); break;
        case(MEMCPY_FAIL):           sprintf(result, "MEMCPY_FAIL"); break;
        case(NOT_MATCH):             sprintf(result, "NOT_MATCH"); break;
        case(NODE_NOT_EXIST):        sprintf(result, "NODE_NOT_EXIST"); break;
        case(UNKONW_MODULE):         sprintf(result, "UNKONW_MODULE"); break;
        case(MODULE_FULL):           sprintf(result, "MODULE_FULL"); break;
        case(UNKNOW_TYPE):           sprintf(result, "UNKNOW_TYPE"); break;
        case(INVALID_MODULE):        sprintf(result, "INVALID_MODULE"); break;
        case(INVALID_TYPE):          sprintf(result, "INVALID_TYPE"); break;
        case(MEMORY_NOT_ENOUGH):     sprintf(result, "MEMORY_NOT_ENOUGH"); break;
        case(BUSY):                  sprintf(result, "BUSY"); break;
        case(NOT_SUPPORTED):         sprintf(result, "NOT_SUPPORTED"); break;
        case(INVALID_VALUE):         sprintf(result, "INVALID_VALUE"); break;
        case(COMING_SOON):           sprintf(result, "COMING_SOON"); break;
        case(INVALID_CHANGE):        sprintf(result, "INVALID_CHANGE"); break;
        case(TIMEOUT):               sprintf(result, "TIMEOUT"); break;
        case(DEVICE_NOT_FOUND):      sprintf(result, "DEVICE_NOT_FOUND"); break;
        case(INVALID_DEVICE):        sprintf(result, "INVALID_DEVICE"); break;
        case(NOT_AUTHORIZED):        sprintf(result, "NOT_AUTHORIZED"); break;
        case(ALREADY):               sprintf(result, "ALREADY"); break;
        case(INTERNAL):              sprintf(result, "INTERNAL"); break;
        case(CONNECTION_FAILED):     sprintf(result, "CONNECTION_FAILED"); break;
        case(ALLOCATION_FAILED):     sprintf(result, "ALLOCATION_FAILED"); break;
        case(OPERATION_FAILED):      sprintf(result, "OPERATION_FAILED"); break;
        case(NOT_AVAILABLE):         sprintf(result, "NOT_AVAILABLE"); break;
        case(CALLBACK_IN_PROGRESS):  sprintf(result, "CALLBACK_IN_PROGRESS"); break;
        case(SERVICE_NOT_FOUND):     sprintf(result, "SERVICE_NOT_FOUND"); break;
        case(DISABLED_BY_USER):      sprintf(result, "DISABLED_BY_USER"); break;
        case(EULA_NOT_ACCEPT):       sprintf(result, "EULA_NOT_ACCEPT"); break;
        case(RUNTIME_NO_RESPONSE):   sprintf(result, "RUNTIME_NO_RESPONSE"); break;
        case(OPENCL_NOT_SUPPORT):    sprintf(result, "OPENCL_NOT_SUPPORT"); break;
        case(NOT_SUPPORT_EYE_TRACKING): sprintf(result, "NOT_SUPPORT_EYE_TRACKING"); break;

        default:
            sprintf(result, "No such error code %i recognized!", error);
    }

    return result;
}

void EyeCallback(ViveSR::anipal::Eye::EyeData_v2 const &eye_data) {
    // Check need for recalibration:
    bool needCalibration = false;
    ViveSR::anipal::Eye::IsUserNeedCalibration(&needCalibration);

    // Store new sample record:
    EnterCriticalSection(&mutex);
    doesNeedCalibration = needCalibration;
    eyeSamples[writeidx % MAX_SAMPLES] = eye_data;
    writeidx++;
    LeaveCriticalSection(&mutex);
}

void shutdown()
{
    if (!initialized)
        return;

    initialized = 0;

    ViveSR::anipal::Eye::UnregisterEyeDataCallback_v2(EyeCallback);
    ViveSR::anipal::Release(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE_V2);

    DeleteCriticalSection(&mutex);
}

int startup()
{
    int error;
    char *version;

    ViveSR::anipal::SRanipal_GetVersion(version);

    error = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE_V2, NULL);
    if (error == ViveSR::Error::WORK) {
        mexPrintf("SRAnipalMex: Successfully initialized eyetracking engine version %s.\n", version);
	return(1);
    }
    else if (error == ViveSR::Error::RUNTIME_NOT_FOUND)
        mexPrintf("SRAnipalMex: SR_Runtime not found.\n");
    else if (error == ViveSR::Error::NOT_SUPPORT_EYE_TRACKING)
        mexPrintf("SRAnipalMex: This HMD does not have eye tracking support\n");
    else
        mexPrintf("SRAnipalMex: Failed to initialize eyetracking engine version %s. Error code %d == %s.\n", version, error, ErrorToString(error));

    return(0);
}

/* This is the main entry point from Matlab/Octave: */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    double* out;
    int i, cmd, result;
    ViveSR::anipal::Eye::EyeData_v2 eye_data;
    bool needCalibration;

    if (nrhs < 1) {
        const char* me = mexFunctionName();
        mexPrintf("SRAnipalMex: A simple MEX file for using the HTC SRAnipal eye tracking runtime\n");
        mexPrintf("with HTC VR HMD's on Microsoft Windows 64-Bit.\n\n");
        mexPrintf("(c) 2023 by Mario Kleiner -- Licensed to you under MIT license.\n");
        mexPrintf("Uses the HTC SRAnipal SDK. See the Psychtoolbox License.txt file in the PsychtoolboxRoot() folder for details.\n");
        mexPrintf("This file is part of Psychtoolbox-3, but should also work independently.\n");
        mexPrintf("\n");
        mexPrintf("Usage:\n\n");
        mexPrintf("success = %s(0);\n", me);
        mexPrintf("- Initialize connection to SRanipal eye tracker. Return success (=1) or failure (=0).\n");
        mexPrintf("After a successfull init, you can call the enable function below to start data collection.\n\n");
        mexPrintf("%s(1); - Shutdown the driver, close connection to tracker.\n\n", me);
        mexPrintf("%s(2); - Clear all pending samples and start data recording into the sample queue.\n\n", me);
        mexPrintf("%s(3); - Stop data recording into the sample queue.\n\n", me);
        mexPrintf("[old, new] = %s(4 [, reqSize]); - Report, and optionally try to resize the internal event queue to 'reqSize' entries.\n", me);
        mexPrintf("Report queue size before/after resize in 'old' and 'new'. You must check 'old' and 'new' for a valid size, as\n");
        mexPrintf("this method can fail if you asked for too much. If it failed, retry with smaller 'reqSize' until success.\n\n");
        mexPrintf("[sample, needsCalibration, trackingImprovements] = %s(5); - Try to fetch the oldest gaze sample from the internal event queue.\n", me);
        mexPrintf("This polls for available data, returning at most one sample from the queue:\n");
        mexPrintf("If no new data is available, it will return an empty matrix, aka isempty(sample) will be true.\n");
        mexPrintf("If data is available, it will return a 33-by-1 double vector with the following elements in the different rows:\n");
        mexPrintf("1 = Serial number. A monotonically increasing serial number. Should increase without gap, otherwise you lost samples!\n");
        mexPrintf("2 = Timestamp. Time of when this sample was taken, in units of milliseconds. Baseline and accuracy are unknown.\n");

        mexPrintf("For the left eye:\n");
        mexPrintf("3 = x-direction, 4 = y-direction, 5 = z-direction of looking vector.\n");
        mexPrintf("6 = x-position, 7 = y-position, 8 = z-position of eye center.\n");
        mexPrintf("9 = Eye openess estimate, normalized to 0-1 range.\n");
        mexPrintf("10 = Pupil diameter in millimeters or NaN for unknown.\n");
        mexPrintf("11 = x-position, 12 = y-position of pupil in sensor area.\n");

        mexPrintf("For the right eye:\n");
        mexPrintf("13 = x-direction, 14 = y-direction, 15 = z-direction of looking vector.\n");
        mexPrintf("16 = x-position, 17 = y-position, 18 = z-position of eye center.\n");
        mexPrintf("19 = Eye openess estimate, normalized to 0-1 range.\n");
        mexPrintf("20 = Pupil diameter in millimeters or NaN for unknown.\n");
        mexPrintf("21 = x-position, 22 = y-position of pupil in sensor area.\n");

        mexPrintf("For the combined eye:\n");
        mexPrintf("23 = x-direction, 24 = y-direction, 25 = z-direction of looking vector.\n");
        mexPrintf("26 = x-position, 27 = y-position, 28 = z-position of eye center.\n");
        mexPrintf("29 = Eye openess estimate, normalized to 0-1 range.\n");
        mexPrintf("30 = Pupil diameter in millimeters or NaN for unknown.\n");
        mexPrintf("31 = x-position, 32 = y-position of pupil in sensor area.\n");
        mexPrintf("33 = Eye convergence distance in millimeters, or NaN for unknown.\n");

        mexPrintf("\n");
        mexPrintf("\n");
        mexPrintf("This driver will only work if the SRAnipal client libraries and runtime are properly installed on your system.\n\n\n");
        return;
    }

    /* First argument must be the command code: */
    cmd = (int) mxGetScalar(prhs[0]);

    if (cmd !=0 && !initialized)
        mexErrMsgTxt("Tried to call SRAnipalMex function, but it isn't initialized yet! Call code 0 (Initialize) first!");

    switch(cmd) {
        case 0: // Init.
            if (initialized)
                mexErrMsgTxt("Tried to initialize SRAnipalMex, but it is already initialized! Call code 1 (Shutdown) first!");

	    plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
	    if (!startup()) {
                *mxGetPr(plhs[0]) = 0;
                break;
            }
            else {
                *mxGetPr(plhs[0]) = 1;
            }

            InitializeCriticalSection(&mutex);
            mexAtExit(shutdown);
            initialized = 1;
            break;

        case 1: // Shutdown.
            shutdown();
            break;

        case 2: // Clear event queue and enable scanning.
            readidx = 0;
            writeidx = 0;
            ViveSR::anipal::Eye::RegisterEyeDataCallback_v2(EyeCallback);
            break;

        case 3: // Disable scanning.
            ViveSR::anipal::Eye::UnregisterEyeDataCallback_v2(EyeCallback);
            break;

        case 4: // Resize queue:
            plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
            // No actual resize yet - Just fixed size of MAX_SAMPLES : if (nrhs >= 2) MAX_SAMPLES = (int) mxGetScalar(prhs[1]);
            plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);
            *mxGetPr(plhs[0]) = (double) MAX_SAMPLES;
            *mxGetPr(plhs[1]) = (double) MAX_SAMPLES;
            break;

        case 5: // Query oldest event packet:
            EnterCriticalSection(&mutex);

            needCalibration = doesNeedCalibration;

            if (writeidx > readidx) {
                eye_data = eyeSamples[readidx % MAX_SAMPLES];
                readidx++;
                result = 1;
            }
            else {
                result = 0;
            }

            LeaveCriticalSection(&mutex);

            if (result) {
                // Yep. Return sample:
                plhs[0] = mxCreateDoubleMatrix(33, 1, mxREAL);
                out = mxGetPr(plhs[0]);

                out[0] = (double) eye_data.frame_sequence;
                out[1] = (double) eye_data.timestamp;
                out[2] = (double) eye_data.verbose_data.left.gaze_direction_normalized.elem_[0];
                out[3] = (double) eye_data.verbose_data.left.gaze_direction_normalized.elem_[1];
                out[4] = (double) eye_data.verbose_data.left.gaze_direction_normalized.elem_[2];
                out[5] = (double) eye_data.verbose_data.left.gaze_origin_mm.elem_[0];
                out[6] = (double) eye_data.verbose_data.left.gaze_origin_mm.elem_[1];
                out[7] = (double) eye_data.verbose_data.left.gaze_origin_mm.elem_[2];
                out[8] = (double) eye_data.verbose_data.left.eye_openness;
                out[9] = (double) (eye_data.verbose_data.left.pupil_diameter_mm >= 0) ? eye_data.verbose_data.left.pupil_diameter_mm : NAN;
                out[10] = (double) eye_data.verbose_data.left.pupil_position_in_sensor_area.elem_[0];
                out[11] = (double) eye_data.verbose_data.left.pupil_position_in_sensor_area.elem_[1];

                out[12] = (double) eye_data.verbose_data.right.gaze_direction_normalized.elem_[0];
                out[13] = (double) eye_data.verbose_data.right.gaze_direction_normalized.elem_[1];
                out[14] = (double) eye_data.verbose_data.right.gaze_direction_normalized.elem_[2];
                out[15] = (double) eye_data.verbose_data.right.gaze_origin_mm.elem_[0];
                out[16] = (double) eye_data.verbose_data.right.gaze_origin_mm.elem_[1];
                out[17] = (double) eye_data.verbose_data.right.gaze_origin_mm.elem_[2];
                out[18] = (double) eye_data.verbose_data.right.eye_openness;
                out[19] = (double) (eye_data.verbose_data.right.pupil_diameter_mm >= 0) ? eye_data.verbose_data.right.pupil_diameter_mm : NAN;
                out[20] = (double) eye_data.verbose_data.right.pupil_position_in_sensor_area.elem_[0];
                out[21] = (double) eye_data.verbose_data.right.pupil_position_in_sensor_area.elem_[1];

                out[22] = (double) eye_data.verbose_data.combined.eye_data.gaze_direction_normalized.elem_[0];
                out[23] = (double) eye_data.verbose_data.combined.eye_data.gaze_direction_normalized.elem_[1];
                out[24] = (double) eye_data.verbose_data.combined.eye_data.gaze_direction_normalized.elem_[2];
                out[25] = (double) eye_data.verbose_data.combined.eye_data.gaze_origin_mm.elem_[0];
                out[26] = (double) eye_data.verbose_data.combined.eye_data.gaze_origin_mm.elem_[1];
                out[27] = (double) eye_data.verbose_data.combined.eye_data.gaze_origin_mm.elem_[2];
                out[28] = (double) eye_data.verbose_data.combined.eye_data.eye_openness;
                out[29] = (double) (eye_data.verbose_data.combined.eye_data.pupil_diameter_mm >= 0) ? eye_data.verbose_data.combined.eye_data.pupil_diameter_mm : NAN;
                out[30] = (double) eye_data.verbose_data.combined.eye_data.pupil_position_in_sensor_area.elem_[0];
                out[31] = (double) eye_data.verbose_data.combined.eye_data.pupil_position_in_sensor_area.elem_[1];

                out[32] = (double) eye_data.verbose_data.combined.convergence_distance_validity ? eye_data.verbose_data.combined.convergence_distance_mm : NAN;

                //out[6] = (double) eye_data.verbose_data.left.eye_data_validata_bit_mask & SINGLE_EYE_DATA_GAZE_ORIGIN_VALIDITY ;
            }
            else {
                // Nope. Return an empty matrix to signal no new sample available:
                plhs[0] = mxCreateDoubleMatrix(0, 0, mxREAL);
            }

            plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);
            out = mxGetPr(plhs[1]);
            out[0] = needCalibration;

            if (!result)
                eye_data.verbose_data.tracking_improvements.count = 0;

            plhs[2] = mxCreateDoubleMatrix(eye_data.verbose_data.tracking_improvements.count, 1, mxREAL);
            out = mxGetPr(plhs[2]);
            for (i = 0; i < eye_data.verbose_data.tracking_improvements.count; i++)
                out[i] = eye_data.verbose_data.tracking_improvements.elem_[i];

            break;

        default:
            mexErrMsgTxt("Invalid / Unknown command code provided as first argument!");
    }

    // Done.
    return;
}
