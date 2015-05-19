/*
    SCREENShowCursorHelper.c

    AUTHORS:

        Allen.Ingling@nyu.edu           awi
        mario.kleiner.de@gmail.com      mk

    PLATFORMS:

        All.

    HISTORY:

        12/14/02    awi     Created.
        10/12/04    awi     Changed "SCREEN" to "Screen" in useString.  Added ShowCursor to see also.  Fixed inappropriate "HideCursor" in this file.
        11/16/04    awi     Added "Helper" suffix.

    DESCRIPTION:

        Shows the mouse pointer

    TO DO:

*/

#include "Screen.h"

#if PSYCH_SYSTEM == PSYCH_LINUX
#include <X11/X.h>
#include <X11/cursorfont.h>
#endif

#if PSYCH_SYSTEM == PSYCH_OSX
#include "PsychCocoaGlue.h"
#endif

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('ShowCursorHelper', screenIndex [, cursorshapeid][, mouseIndex]);";
//                                                    1              2                3
static char synopsisString[] =
    "This is a helper function called by ShowCursor.  Do not call Screen(\'ShowCursorHelper\'), use "
    "ShowCursor instead.\n"
    "Show the mouse pointer. If optional 'cursorshapeid' is given, select a specific cursor shape as well.\n"
    "If optional 'mouseIndex' is given, setup cursor for given master mouseIndex device (Linux only).\n";
static char seeAlsoString[] = "HideCursorHelper";

PsychError SCREENShowCursorHelper(void)
{
    int	screenNumber, cursorid, mouseIdx;
    char *cursorName = NULL;

#if PSYCH_SYSTEM == PSYCH_LINUX
    Cursor  mycursor;
#endif

    //all subfunctions should have these two lines.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumInputArgs(3));   //The maximum number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs

    PsychCopyInScreenNumberArg(1, TRUE, &screenNumber);

    mouseIdx = -1;
    PsychCopyInIntegerArg(3, FALSE, &mouseIdx);

    PsychShowCursor(screenNumber, mouseIdx);

    // Copy in optional cursor shape id argument: The default of -1 means to
    // not change cursor appearance. Any other positive value changes to an
    // OS dependent shape (== the mapping of numbers to shapes is OS dependent).
    cursorid = -1;
    if (!PsychCopyInIntegerArg(2, kPsychArgAnything, &cursorid)) {
        if (PsychAllocInCharArg(2, kPsychArgAnything, &cursorName)) cursorid = -2;
    }

    // Cursor change request?
    if (cursorid!=-1) {
        // Yes.
#if PSYCH_SYSTEM == PSYCH_OSX
        // OS/X:
        PsychCocoaSetThemeCursor(cursorid);
#endif

#if PSYCH_SYSTEM == PSYCH_LINUX

        #ifdef PTB_USE_WAYLAND
            // GNU/Linux with Wayland backend:

            // Map X11 id's to Wayland equivalents:
            switch (cursorid) {
                case 26: // SandClock
                    cursorName = "watch";
                break;

                case 58: // Hand
                    cursorName = "hand1";
                    break;

                case 34: // CrossHair
                    cursorName = "crosshair";
                    break;

                case 1: // IBeam
                    cursorName = "xterm";
                    break;

                case 0: // Arrow
                    cursorName = "left_ptr";
                    break;

                case 2: // Arrow
                    cursorName = "left_ptr";
                    break;

                case 5: // Arrow
                    cursorName = "left_ptr";
                    break;

                case -2: // Special case: Use provided cursorName string, if any:
                    if (cursorName) break;

                default:
                    cursorName = "left_ptr";
            }

            PsychOSDefineWaylandCursor(screenNumber, mouseIdx, cursorName);
        #else
            // GNU/Linux with X11 windowing system:
            // Map screenNumber to X11 display handle and screenid:
            CGDirectDisplayID dpy;
            PsychGetCGDisplayIDFromScreenNumber(&dpy, screenNumber);
            // Create cursor spec from passed cursorid:
            mycursor = XCreateFontCursor(dpy, (unsigned int) cursorid);
            if (mouseIdx < 0) {
                // Set cursor for our window:
                PsychOSDefineX11Cursor(screenNumber, -1, mycursor);
            } else {
                // XInput cursor: Master pointers only.
                int nDevices;
                XIDeviceInfo* indevs = PsychGetInputDevicesForScreen(screenNumber, &nDevices);

                // Sanity check:
                if (NULL == indevs) PsychErrorExitMsg(PsychError_user, "Sorry, your system does not support individual mouse pointers.");
                if (mouseIdx >= nDevices) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such cursor pointer.");
                if (indevs[mouseIdx].use != XIMasterPointer) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such master cursor pointer.");

                PsychOSDefineX11Cursor(screenNumber, indevs[mouseIdx].deviceid, mycursor);
            }

            XFlush(dpy);
        #endif
        // Done (hopefully).
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
        // Microsoft Windows:
        LPCTSTR lpCursorName;

        #ifndef IDC_HAND
        #define IDC_HAND MAKEINTRESOURCE(32649)
        #endif

        // Map provided cursor id to a Windows system id for such a cursor:
        switch(cursorid) {
            case 0:
                // Standard arrow cursor:
                lpCursorName = IDC_ARROW;
                break;

            case 1:
                // haircross cursor:
                lpCursorName = IDC_CROSS;
                break;

            case 2:
                // hand cursor:
                lpCursorName = IDC_HAND;
                break;

            case 3:
                // Arrows in all 4 directions cursor:
                lpCursorName = IDC_SIZEALL;
                break;

            case 4:
                // north-south cursor:
                lpCursorName = IDC_SIZENS;
                break;

            case 5:
                // east-west cursor:
                lpCursorName = IDC_SIZEWE;
                break;

            case 6:
                // hourglass cursor:
                lpCursorName = IDC_WAIT;
                break;

            case 7:
                // No cursor:
                lpCursorName = IDC_NO;
                break;

            case 8:
                // IBeam/text cursor:
                lpCursorName = IDC_IBEAM;
                break;

            default:
                // Default for unknown id is the standard arrow cursor:
                lpCursorName = IDC_ARROW;
        }

        // Load and set a cursor, based on the selected lpCursorName cursor id string:
        SetCursor(LoadCursor(NULL, lpCursorName));
#endif
        // End of cursor shape setup.
    }

    return(PsychError_none);
}
