function oldType = ShowCursor(type)
% oldType = ShowCursor([type])
%
% ShowCursor redisplays the mouse pointer after a previous call to
% HideCursor.
% 
% OSX, WIN, LINUX: _______________________________________________________
%
% The 'type' argument specifying the cursor shape is ignored; The
% cursor shape can not be set.
%
% OS9: ___________________________________________________________________ 
%
% ShowCursor.mex asks the Macintosh OS to cancel one call to HideCursor.
% You must call ShowCursor at least as many times as you've called
% HideCursor before the cursor will appear. Excess calls to ShowCursor are
% ignored. (When Screen closes its last window it calls InitCursor which
% zeroes the count.)
%
% If provided, the optional "type" argument changes the cursor shape to:
%   0: Arrow
%   1: I Beam
%   2: Cross
%   3: Plus
%   4: Watch
%   5: Arrow
% 128: P
% 300: Beachball 1/4
% 301: Beachball 2/4
% 302: Beachball 3/4
% 303: Beachball 4/4
% 400: fat arrow
% 401: fat I Beam
% Type 0 (and 5 for backward compatibility) is predefined as the standard 
% arrow cursor. The rest return whatever Apple's GetCursor(type) finds in
% the  System or Matlab's resource forks. If nothing is found, the type is
% reset to 0. The fat arrow and I beam are copied from the "Fat Cursors v
% 1.2" control panel created by Robert Abatecola, 5106 Forest Glen Drive,
% San Jose, CA 95129.
% _________________________________________________________________________

% 7/23/97  dgp Cosmetic editing.
% 8/15/97  dgp Explain hide/show counter.
% 3/15/99  xmz Added comments for PC version.
% 8/19/00  dgp Cosmetic.
% 4/14/03  awi ****** OS X-specific fork from the OS 9 version *******
%               Added call to Screen('ShowCursor'...) for OS X.
% 7/12/04  awi Divided into sections by platform.
% 11/16/04 awi Renamed Screen("ShowCursor") to Screen("ShowCursorHelper").
% 10/4/05	  awi Note here that dgp made unnoted cosmetic changes between 11/16/04 and 10/4/05.


% Use Screen to emulate ShowCursor.mex
Screen('ShowCursorHelper',0);
