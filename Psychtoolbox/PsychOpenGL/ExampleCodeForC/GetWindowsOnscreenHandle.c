/* GetWindowsOnscreenHandle.c -- Minimal example of how to
 * retrieve a GDI Window handle HWND for a specific Psychtoolbox
 * onscreen window. This is M$-Windows only!
 *
 * This simple example expects the Psychtoolbox windowIndex for an
 * open PTB onscreen window. It then retrieves a HWND GDI handle for
 * that window by synthesizing the Windows caption string from the
 * windowIndex winid, and using FindWindow() to find the window by name.
 * on success, it calls HideWindow() to hide that window, just as a proof
 * it actually works.
 *
 * Compile with: mex GetWindowsOnscreenHandle.c user32.dll
 *
 * Written by Mario Kleiner.
 */

#include <windows.h>
#include <stdlib.h>
#include "mex.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
	HWND ptbwin;	
	char winName[100];
	int winid = 10;
	int show = 0;
	if(nrhs<1) {
		mexPrintf("No windowIndex provided, will default to 10.");
	}
	else winid = (int) mxGetScalar(prhs[0]);
	
	sprintf(winName, "PTB Onscreen window [%i]:", winid);
	ptbwin = FindWindow("PTB-OpenGL", winName);
	if (ptbwin!=NULL) {
   	mexPrintf("SUCCESS for Window %i!\n", winid);
		if(nrhs>1) show = (int) mxGetScalar(prhs[1]);
		ShowWindow(ptbwin, (show) ? SW_SHOW : SW_HIDE);
   }
	else {
		mexPrintf("FAILED!\n");
   }
}
