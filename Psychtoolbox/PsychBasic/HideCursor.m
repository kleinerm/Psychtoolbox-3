function HideCursor
% HideCursor
% 
% HideCursor.mex hides the cursor. Use ShowCursor to make it visible
% again.
% 
% OS 9: ___________________________________________________________________
%
% Flushing HideCursor.mex or Screen.mex (eg "clear hidecursor",  "clear
% screen", "clear mex", or "clear all") makes the cursor visible, by a call
% to InitCursor.
%
% Note that the Mac OS counts the number of times you call HideCursor, and
% requires an equal number of calls to ShowCursor before the cursor becomes
% visible again. Excess calls to ShowCursor are ignored. (When Screen
% closes its last window it calls InitCursor, which  resets the count.)
% 
% HideCursor vs. QuickDEX: If you leave QuickDEX (the old address-book
% desk accessory) running in the background while running Matlab, then
% HideCursor.mex doesn't work. HideCursor returns normally, but the cursor
% remains visible. This seems to be a bug in QuickDEX, but the program
% hasn't been sold for many years, so an update is unlikely. The work
% around is simply to quit QuickDEX before using HideCursor in Matlab.
% 
% WARNING: When BACKGROUNDING is enabled, Matlab shows the cursor
% before executing each Matlab statement, even if you just called
% HideCursor. So turn off BACKGROUNDING:
% 
% Screen('Preference','Backgrounding',0); 
% % Fast in Matlab 5.2.1, slow in earlier versions.
%
% WARNING: When you run your Matlab code as a script, rather than as a
% FUNCTION,  it appears that Matlab shows the cursor before executing some
% Matlab statements. So put the FUNCTION statement in the first line of
% your M file. Thanks to Thomas Jerde  <thomasjerde@hotmail.com> for
% reporting this. 
% web http://groups.yahoo.com/group/psychtoolbox/message/1186;
%
% _________________________________________________________________________
%
% See ShowCursor, CursorTest, Screen Preference Backgrounding.

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

%on OS X the Screen.mexmac hides the cursor, not 
%HideCursor.mexmac.  HideCursor.m wraps the 
%Screen call to emulate HideCursor.mex
Screen('HideCursorHelper',0);
