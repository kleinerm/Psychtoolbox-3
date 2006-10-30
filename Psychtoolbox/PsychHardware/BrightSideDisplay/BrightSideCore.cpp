/* BrightSideCore.c -- Source code of the BrightSideCore.dll
 * Matlab MEX file.
 *
 * BrightSideCore.dll is the low-level interface to BrightSide Technologies
 * core libraries. It is usually called by the high-level Psychtoolbox Matlab
 * M-File BrightSideHDR.m
 *
 * Build instructions:
 * mex -v -g -IU:\test\hdr4matlab -LU:\test\hdr4matlab -lGL_OutputLibrary.lib BrightSideCore.c
 * Requires:
 *
 * Copyright 2006 Mario Kleiner - MPI for Biological Cybernetics, Tuebingen, Germany.
 * This file is licensed to you under the GNU General Public License. See the
 * License.txt file in the Psychtoolbox root folder.
 */

// We use the standard namespace:
using namespace std;

// Matlab MEX-API definition:
#include "mex.h"

// Standard system includes:
#include <stdlib.h>
#include <stdio.h>

/* Most probably not needed files:
#include <math.h>

#include "hdrio.h"
#include "yxy.h"
#include "log.h"
#include "hist.h"
#include "norm.h"
*/

// Include the BrightSide core library header:
#include "GLOutputLibrary.h"

// A simple shortcut:
#define DCGI	DisplayController::GetInstance()

// Main entry point and interface to Matlab, our main() routine:
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    int cmd;
    char basepath[FILENAME_MAX];
    char configname[FILENAME_MAX];
    
    // Child protection:
    if (nlhs>0) mexErrMsgTxt("BrightSideCore: Superfluous return argument provided!\n");
    if (nrhs<1) mexErrMsgTxt("BrightSideCore: Missing command code!\n");
    if (!mxIsDouble(prhs[0])) mexErrMsgTxt("BrightSideCore: Invalid (non-int) command code!\n");

    // Retrieve command code:
    cmd = (int) mxGetScalar(prhs[0]);

    // Dispatch:
    switch(cmd) {
        case -1: // Dummy init. Do nothing. This is just to force dynamic loading and linking of
                 // the MEX file.
           mexPrintf("BrightSideCore: Loaded.\n");
        break;

        case 0: // Initialization of HDR core library.
            mexPrintf("BrightSideCore: Starting up...\n");
            if (nrhs<3 || !mxIsChar(prhs[1]) || !mxIsChar(prhs[2])) {
                mexErrMsgTxt("BrightSideCore: Missing or invalid basepath or name for config file!\n");
            }

            if (nrhs<5 || !mxIsDouble(prhs[3]) || !mxIsDouble(prhs[4])) {
                mexErrMsgTxt("BrightSideCore: Missing or invalid source texid or target fbo id!\n");
            }

            // Retrieve path and config file name:
            mxGetString(prhs[1], basepath, size(basepath));
            mxGetString(prhs[2], configname, size(basepath));

            // Initialize Brightside display controller and core library:
            // BrightSide::DCGI.Initialize( "../", "DR-37P-beta.xml" );
            BrightSide::DCGI.Initialize(basepath, configname);

            // When display is called, the texture in tex will be rendered to the back buffer
            BrightSide::DCGI.SetInputOutput((unsigned int) mxGetScalar(prhs[3]), (unsigned int) mxGetScalar(prhs[4]));
            // Ready!
        break;
        
        case 1: // Shutdown of HDR core library.
            mexPrintf("BrightSideCore: Shutdown request...\n");
            BrightSide::DCGI.Unload();
        break;
        
        case 2: // Convert & Blit request to library:
            mexPrintf("BrightSideCore: Conversion blit request...\n");
            BrightSide::DCGI.Display();
        break;
        
        default:
            mexErrMsgTxt("BrightSideCore: Unknown command code provided!\n");
    }

    // Done. Return control to Matlab:
    return;
}
