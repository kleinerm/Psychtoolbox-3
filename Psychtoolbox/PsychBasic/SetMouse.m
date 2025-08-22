function SetMouse(x,y,windowPtrOrScreenNumber, mouseid, detachFromMouse)
% SetMouse(x, y [, windowPtrOrScreenNumber][, mouseid][, detachFromMouse=0])
% 
% Position the mouse cursor on the screen.
%
% The cursor position (x,y) is "local" to the screen, i.e. relative to the
% origin of the screen if a screen number is supplied, or relative to the
% origin of a screen on which a supplied onscreen window is displayed.
% Otherwise it's "global", i.e. relative to the origin of the main screen
% (Screen 0). It is advisable to specify an onscreen window handle for
% proper handling of Retina displays on macOS if you use backwards
% compatibility mode. Note: (x,y) is always local to the onscreen window on
% Linux with Wayland, as of the year 2025, a stark difference to all other systems!
%
% On Linux with X11, the optional 'mouseid' parameter allows to select
% which of potentially multiple cursors should be repositioned. On OS/X and
% Windows this parameter is silently ignored.
%
% On macOS, the optional 'detachFromMouse' parameter, if set to 1 instead of
% its default value of zero, will detach mouse cursor movements from mouse
% movements. The cursor will be frozen in place and can only be moved via
% SetMouse(), but no longer via mouse movements. On Linux and Windows this
% parameter is currently silently ignored.
%
% On Linux with the Wayland backend, this function is only supported by some
% Wayland based GUI's, but not others. On systems lacking SetMouse() support,
% a warning message will be printed. On systems with support, it is up to the
% display server if the SetMouse() request is actually honored, or ignored, but
% in general it only works while an onscreen window has pointer focus, and then
% the provided (x,y) position is local to the onscreen window, not to the screen,
% as opposed to how it works on non-Wayland systems! This is sadly unavoidable, due
% to Waylands refusal to operate anything in desktop global coordinates.
% The following link may give some info about which Wayland GUI's support mouse
% cursor positioning: https://wayland.app/protocols/pointer-warp-v1#wp_pointer_warp_v1
%
% The delay between a call to SetMouse and when GetMouse will report the
% new mouse cursor position is not known. GetMouse seems to report the new
% position immediately, but we have no guarantee that it always will.
%
% _________________________________________________________________________
%
% See Also: GetMouse, GetClicks

% 6/7/96    dhb     Wrote it.
% 8/23/96   dhb     Added support for windowInfo argument.
% 3/23/97   dgp     Updated.
% 8/14/97   dhb     Added comment about delay.
% 8/15/97   dgp     Suggest WaitTicks(1).
% 4/24/01   awi     Added WINDOWS section.
% 6/10/01   awi     Added See Also.  
% 4/14/03   awi     ****** OS X-specific fork from the OS 9 version *******
%                   Added call to Screen('PositionCursor'...) for OS X.
% 10/12/04  awi     Cosmetic changes to help.  This file should be modified 
%                   after the great mouse shift to state SetMouse is deprecated.
% 11/18/04  awi     Renamed "PositionCursor" to "PositionCursorHelper".
% 02/21/06  mk      Added Linux support.
% 06/17/06  mk      Added Windows support.
% 11/04/14  mk      round() x,y coords for integral coordinates to avoid error.
% 04/18/15  mk      Update help text - local coordinates now should also
%                   work on MS-Windows, not only Linux and OSX.
% 07/20/15  mk      Add support for 'detachFromMouse', on OSX for now.

% SetMouse.m wraps the Screen('PositionCursor',..) call to emulate the old SetMouse.mex

if nargin < 2
  error('SetMouse requires x and y positions');
end

if nargin < 3 || isempty(windowPtrOrScreenNumber)
  windowPtrOrScreenNumber = 0;
end

if nargin < 4
  mouseid = [];
end

if nargin < 5 || isempty(detachFromMouse)
  detachFromMouse = 0;
end

% OSX handling of Retina displays:
if IsOSX && (Screen('WindowKind', windowPtrOrScreenNumber) == 1)
    winfo = Screen('GetWindowInfo', windowPtrOrScreenNumber);
    if ~winfo.IsFullscreen || (Screen('Preference', 'WindowShieldingLevel') < 2000) || winfo.SysWindowHandle > 0
        % Cocoa - Half the factor is right:
        isf = winfo.ExternalMouseMultFactor / 2;
    else
        % CGL fullscreen:
        isf = winfo.ExternalMouseMultFactor;
    end

    x = x * isf;
    y = y * isf;
end

if IsLinux && (Screen('WindowKind', windowPtrOrScreenNumber) == 1) && ~IsWayland
    rect = Screen('GlobalRect', windowPtrOrScreenNumber);
    x = x + rect(1);
    y = y + rect(2);
end

Screen('SetMouseHelper', windowPtrOrScreenNumber, round(x), round(y), mouseid, detachFromMouse);
