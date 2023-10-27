function HideCursor(screenid, mouseid)
% HideCursor([screenidOrWindow=0][, mouseid])
% 
% HideCursor hides the mouse cursor associated with 'screenidOrWindow'.
%
% 'screenidOrWindow' allows to specify the screen or onscreen window to which
% the function should apply.
%
% By default, the cursor on screen zero on Linux/X11, and on all screens on
% Windows and Mac OS/X is hidden. Although optional, it is strongly recommended
% to provide this parameter for cross-platform compatibility across operating
% systems.
%
% Note that this function may not have any effect if the cursor location is not
% on top of an open onscreen window, as cursor visibility or shape may not be
% under Psychtoolbox control while the cursor interacts with other applications
% windows. It may also do nothing if you call the function while no onscreen
% window is open at all. For this reason, you should place calls to HideCursor
% after the calls to Screen('OpenWindow') or PsychImaging('OpenWindow'), not
% before them.
%
% 'mouseid' defines which of multiple cursors shall be hidden on Linux/X11. The
% parameter is silently ignored on other systems.
% _________________________________________________________________________
%
% See ShowCursor, SetMouse

% 7/23/97  dgp Added wish.
% 8/15/97  dgp Explain hide/show counter.
% 3/27/99  dgp Mention Backgrounding.
% 3/28/99  dgp Show how to turn off backgrounding. 
% 1/22/00  dgp Cosmetic.
% 4/25/02  dgp Mention conflict with QuickDEX.
% 4/14/03  awi ****** OS X-specific fork from the OS 9 version *******
%               Added call to Screen('HideCursor'...) for OS X.
% 7/12/04  awi Cosmetic and uses IsMac.
% 11/16/04 awi Renamed "HideCursor" to "HideCursorHelper"

if (nargin < 1) || isempty(screenid)
  screenid = 0;
end

if nargin < 2
  mouseid = [];
end

Screen('HideCursorHelper', screenid, mouseid);
