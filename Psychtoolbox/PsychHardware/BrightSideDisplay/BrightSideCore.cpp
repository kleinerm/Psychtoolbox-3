/* BrightSideCore.cpp -- Source code of the BrightSideCore.dll
 * Matlab MEX file.
 *
 * BrightSideCore.dll is the low-level interface to BrightSide Technologies
 * core libraries. It is usually called by the high-level Psychtoolbox Matlab
 * M-File BrightSideHDR.m
 *
 * Build instructions:
 * 1. Change into the Psychtoolbox/PsychHardware/BrightSideDisplay folder.
 * 2. mex -v -output BrightSideCore -IBSRuntimeLibs\outputlib\include BSRuntimeLibs\outputlib\lib\GL_OutputLibrary.lib BrightSideCore.cpp
 *
 * Requires:
 * - A recent C++ compiler to handle the complex templates in the BrightSide libraries.
 * - The BrightSide SDK and runtimes to be installed in the BSRuntimeLibs subfolder,
 *   see the Contents.m file in that folder for more info.
 *
 * Copyright 2006 Mario Kleiner - MPI for Biological Cybernetics, Tuebingen, Germany.
 * This file is licensed to you under the GNU General Public License. See the
 * License.txt file in the Psychtoolbox root folder.
 */

// Matlab MEX-API definition:
#include "mex.h"

// Standard system includes:
#include <stdlib.h>
#include <stdio.h>

// Include the BrightSide core library header:
#include "GLOutputLibrary.h"

// A simple shortcut:
#define DCGI	DisplayController::GetInstance()

// Main entry point and interface to Matlab, our main() routine:
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    int cmd;
    static int debuglevel = 0;
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
	         // the MEX file, and to set the debuglevel, if provided.
	    if (nrhs>1 && mxIsDouble(prhs[1])) debuglevel = (int) mxGetScalar(prhs[1]);
	    
	    if (debuglevel>0) mexPrintf("BrightSideCore: Loaded.\n");
	break;

        case 0: // Initialization of HDR core library.
            if (debuglevel>0) mexPrintf("BrightSideCore: Starting up...\n");

	    if (nrhs<3 || !mxIsChar(prhs[1]) || !mxIsChar(prhs[2])) {
	      mexErrMsgTxt("BrightSideCore: Missing or invalid basepath or name for config file!\n");
            }

            if (nrhs<5 || !mxIsDouble(prhs[3]) || !mxIsDouble(prhs[4])) {
	      mexErrMsgTxt("BrightSideCore: Missing or invalid source texid or target fbo id!\n");
            }
	    
            // Retrieve path and config file name:
            mxGetString(prhs[1], basepath, sizeof(basepath));
            mxGetString(prhs[2], configname, sizeof(basepath));

            // Initialize Brightside display controller and core library:
            // BrightSide::DCGI.Initialize( "../", "DR-37P-beta.xml" );
            BrightSide::DCGI.Initialize(basepath, configname);

            // When display is called, the texture in tex will be rendered to the back buffer
            BrightSide::DCGI.SetInputOutput((unsigned int) mxGetScalar(prhs[3]), (unsigned int) mxGetScalar(prhs[4]));
            // Ready!
        break;
        
        case 1: // Shutdown of HDR core library.
            if (debuglevel>0) mexPrintf("BrightSideCore: Shutdown request...\n");
            BrightSide::DCGI.Unload();
        break;
        
        case 2: // Convert & Blit request to library:
            if (debuglevel>0) mexPrintf("BrightSideCore: Conversion blit request...\n");
            BrightSide::DCGI.Display();
        break;
        
        case 3: // Change rendersource and target for conversion library:
            if (debuglevel>0) mexPrintf("BrightSideCore: Change of render source and target...\n");

            if (nrhs<3 || !mxIsDouble(prhs[1]) || !mxIsDouble(prhs[2])) {
                mexErrMsgTxt("BrightSideCore: Missing or invalid source texid or target fbo id!\n");
            }

            // When display is called, the texture in tex will be rendered to the given framebuffer:
            BrightSide::DCGI.SetInputOutput((unsigned int) mxGetScalar(prhs[1]), (unsigned int) mxGetScalar(prhs[2]));
        break;

        case 4: // Change LED intensity multiplier:
            if (debuglevel>0) mexPrintf("BrightSideCore: Change of LED intensity...\n");

            if (nrhs<2 || !mxIsDouble(prhs[1])) {
                mexErrMsgTxt("BrightSideCore: Missing or invalid LED intensity!\n");
            }

            // When display is called, the texture in tex will be rendered to the given framebuffer:
            BrightSide::DCGI.LEDIntensity((float) mxGetScalar(prhs[1]));
        break;

        default:
	  mexErrMsgTxt("BrightSideCore: Unknown command code provided!\n");
    }

    // Done. Return control to Matlab:
    return;
}
