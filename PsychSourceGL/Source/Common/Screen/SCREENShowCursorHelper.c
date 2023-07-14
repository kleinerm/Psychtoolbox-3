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

PsychError SCREENShowCursorHelper(void)
{
    // If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
    static char useString[] = "Screen('ShowCursorHelper', screenIndex [, cursorshapeid][, mouseIndex]);";
    //                                                    1              2                3
    static char synopsisString[] =
    "This is a helper function called by ShowCursor.  Do not call Screen(\'ShowCursorHelper\'), use "
    "ShowCursor instead.\n"
    "Show the mouse pointer. If optional 'cursorshapeid' is given, select a specific cursor shape as well.\n"
    "If optional 'mouseIndex' is given, setup cursor for given master mouseIndex device (Linux only).\n";
    static char seeAlsoString[] = "HideCursorHelper";

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
                    if (cursorName)
                        break;
                    // fallthrough
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
                // Double-pointed arrow pointing north and south:
                lpCursorName = IDC_SIZENS;
                break;

            case 5:
                // Double-pointed arrow pointing west and east:
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

            case 9:
                // Double-pointed arrow pointing northeast and southwest:
                lpCursorName = IDC_SIZENESW;
                break;

            case 10:
                // Double-pointed arrow pointing northwest and southeast:
                lpCursorName = IDC_SIZENWSE;
                break;

            case 11:
                // Standard arrow and small hourglass:
                lpCursorName = IDC_APPSTARTING;
                break;

            case 12:
                // Arrow and question mark:
                lpCursorName = IDC_HELP;
                break;

            case 13:
                // Vertical arrow:
                lpCursorName = IDC_UPARROW;
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

PsychError SCREENConstrainCursor(void)
{
    static char useString[] = "Screen('ConstrainCursor', windowIndex, addConstraint [, rect]);";
    //                                                   1            2                3
    static char synopsisString[] =
    "Confine mouse cursor position to a specified area inside onscreen window 'windowIndex'.\n\n"
    "If you set 'addConstraint' to 1, then a region constraint is added: 'rect' specifies the "
    "rectangle (in window local coordinates) to which the mouse cursor should be confined. If "
    "you omit 'rect', then the cursor is confined to the region of the window, ie. can't leave "
    "the window. On MS-Windows you can only define one single rectangular region at all, regardless "
    "of the number of onscreen windows. On Linux/X11 you can define up to a total of (currently) 1024 "
    "confinement regions, e.g., for multiple separate windows, or multiple regions per window.\n"
    "Additionally on Linux/X11 you can define empty 'rect's which define a horizontal or vertical line. "
    "This adds a horizontal or vertical border line which can not be crossed by the mouse cursor, so you "
    "could, e.g., build a complex maze, in which the cursor has to navigate. Please note that this "
    "ability will not be present on a future version of Psychtoolbox for Linux with the Wayland display "
    "server. While the Wayland implementation will provide the ability to define multiple regions, its "
    "semantic will very likely be different, so if you use this special Linux/X11 only feature, your code "
    "will not only be non-portable to MS-Windows, but also to future Linux versions which use Wayland instead "
    "of the X11 graphics system!\n\n"
    "If you set 'addConstraint' to 0 and specify a 'rect', then the specified 'rect' confinement region "
    "for the given onscreen window is removed on Linux/X11. If you omit 'rect' on Linux, then all confinement "
    "regions for the given onscreen window are removed. On MS-Windows the single globally available confinement "
    "region is removed if it was set for the given onscreen window, regardless if you specify 'rect' or not, "
    "as there is no ambiguity or choice with only one global rect anyway.\n\n"
    "Closing an onscreen window with active cursor constraints will automatically remove all associated "
    "cursor confinement regions. This is true for proper close via Screen('Close', windowIndex), Screen('Closeall') or sca, "
    "or during a controlled error abort with proper error handling. On Linux, quitting/killing or crashing Octave/Matlab "
    "will also release pointer confinement. On MS-Windows, pressing ALT+TAB will release the confinement.\n\n"
    "The 'ConstrainCursor' function is not currently supported or supportable on Apple macOS due to macOS "
    "operating system limitations. See 'help SetMouse' sections referring to the 'detachFromMouse' parameter for "
    "a hint on how you may be able to work around this macOS limitation for some applications.\n\n";
    static char seeAlsoString[] = "HideCursorHelper";

    PsychWindowRecordType *windowRecord;
    int addConstraint;
    PsychRectType rect;

    // All subfunctions should have these two lines.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    PsychErrorExit(PsychCapNumInputArgs(3));            // The maximum number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(0));           //The maximum number of outputs

    // Get windowRecord:
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    if (!PsychIsOnscreenWindow(windowRecord))
        PsychErrorExitMsg(PsychError_user, "Specified window is not an onscreen window, as required.");

    // Get flag:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &addConstraint);

    if (addConstraint) {
        // If optional rect is omitted, use full window rect:
        if (!PsychCopyInRectArg(3, kPsychArgOptional, rect))
            PsychCopyRect(rect, windowRecord->rect);

        if (!ValidatePsychRect(rect) || rect[kPsychLeft] < 0 || rect[kPsychTop] < 0 ||
            rect[kPsychRight] > PsychGetWidthFromRect(windowRecord->rect) ||
            rect[kPsychBottom] > PsychGetHeightFromRect(windowRecord->rect)) {
            PsychErrorExitMsg(PsychError_user, "Invalid 'rect' provided. Invalid, or reaches outside the onscreen windows borders.");
        }

        // Add a new cursor constraint for this window, defined by rect:
        if (!PsychOSConstrainPointer(windowRecord, TRUE, rect))
            PsychErrorExitMsg(PsychError_user, "Failed to add cursor constraint for onscreen window.");
    }
    else {
        if (PsychCopyInRectArg(3, kPsychArgOptional, rect)) {
            // Remove cursor constraint for this window, as defined by rect:
            if (!PsychOSConstrainPointer(windowRecord, FALSE, rect))
                PsychErrorExitMsg(PsychError_user, "Failed to remove cursor constraint for onscreen window.");
        } else {
            // Remove all cursor constraints for this window:
            if (!PsychOSConstrainPointer(windowRecord, FALSE, NULL))
                PsychErrorExitMsg(PsychError_user, "Failed to remove all cursor constraints for onscreen window.");
        }
    }

    return(PsychError_none);
}
