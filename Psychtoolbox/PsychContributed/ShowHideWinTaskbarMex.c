/*------------------------------------------------------------------------------
 * ShowHideWinTaskbarMex.c -- A simple Matlab MEX file for showing/hiding the
 * taskbar on Microsoft Windows.
 *
 * Usage:
 * ShowHideWinTaskbarMex(1) -- To show the taskbar.
 * ShowHideWinTaskbarMex(0) -- To hide the taskbar.
 *
 * Compile with:
 *
 * mex -v ShowHideWinTaskbarMex.c user32.lib
 *
 * If you want to compile on Octave, do this:
 * mex -v ShowHideWinTaskbarMex.c user32.lib -DPTBOCTAVE3MEX
 */

/* Windows includes: */
#include <windows.h>

/* Matlab includes: */
#include "mex.h"

void showHide(int show)
{
    HWND hWnd;
    hWnd = FindWindow("Shell_TrayWnd", NULL);
    if (hWnd) {
        ShowWindow(hWnd, (show) ? SW_SHOW : SW_HIDE);
    } else {
        mexPrintf("OOPS! Couldn't find taskbar window. Failed!\n");
    }
}

void exitfunc(void)
{
    showHide(1);
}

/* This is the main entry point from Matlab: */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])
{
    int show = 1;
    
    mexAtExit(exitfunc);    
    if (nrhs > 0) show = (int) mxGetScalar(prhs[0]);
    showHide(show);
    return;
}
