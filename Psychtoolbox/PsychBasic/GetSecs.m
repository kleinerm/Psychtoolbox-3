function s=GetSecs(s,subscript)
% s=GetSecs
% 
% GetSecs returns the time in seconds (with high precision) since the
% computer started. GetSecs is an improved replacement for Matlab's
% CPUTIME, which is unreliable.
%
% Call GetSecsTick to find out the precision of the clock used by GetSecs.
% 
% TIMING ADVICE: the first time you access any MEX function or M file,
% Matlab takes several hundred milliseconds to load it from disk.
% Allocating a variable takes time too. Usually you'll want to omit
% those delays from your timing measurements by making sure all the
% functions you use are loaded and that all the variables you use are
% allocated, before you start timing. MEX files stay loaded until you
% flush the MEX files (e.g. by changing directory or calling CLEAR
% MEX). M files and variables stay in memory until you clear them.
% 
% CPUTIME is a Matlab built-in function that is nominally similar to
% GetSecs. Both return absolute time in seconds. A minor difference is
% that GetSecs returns time since System startup, whereas CPUTIME
% returns time since Matlab started. The important difference is that
% CPUTIME was utterly unreliable in Matlab 4, and its help note in
% Matlab 5 says "The return value may overflow the internal
% representation and wrap around." We suggest that you avoid CPUTIME, 
% and use GetSecs instead.
%
% Win : ___________________________________________________________________
%
% On Windows machines the high precision GetPerformanceCounter() call 
% is used to get the number of seconds since system start up, if a 
% performance counter is available. Otherwise, the less accurate 
% GetCurrentTick() is used. 
% 
% See also: GetSecsTick, WaitSecs, GetTicks, WaitTicks, GetSecsTest, 

% 3/15/97  dgp  Expanded comments.
% 4/9/97   dgp  Documented the optional arguments.
% 2/17/99  dgp  Typo: replaced s[subscript] by s(subscript).
% 3/15/99  dgp  Mention SecondsMultiplier.
% 3/15/99  xmz  Put in comments for Windows version.
% 3/23/99  dgp  Cosmetic.
% 4/10/99  dgp  Say more about Mac OS Time Manager.
% 4/19/99  dgp  Begin with an executive summary, as suggested by Allen Ingling.
% 2/4/00   dgp  Updated for Mac OS 9.
% 6/16/00  dgp  Cosmetic.
% 10/25/05 awi  Divided into general section and OS 9 & Win specific sections.
%               Imported into OS X PTB. 
