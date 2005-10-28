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
% OS 9: ___________________________________________________________________
% 
% CALIBRATION: On most Power Macs, GetSecs uses an extremely stable
% timebase (built into the PowerPC processor) but it may run a bit
% fast or slow (around 1%). You may wish to run GetSecsTest, which
% will calibrate your particular PowerPC chip's rate, and improve
% timing accuracy to within 0.01%. The correction applies to all the
% Psychtoolbox functions (GetSecs, WaitSecs, Screen PeekBlanking,
% etc.) that use that timebase. See GetSecsTest.
%  
% THREE TIME BASES
% 
% GetSecs uses the extremely stable PowerPC processor clock if the
% UpTime routine is available (PowerMac with Mac OS 8.6 or better). 
% Otherwise, GetSecs uses the Time Manager's Microseconds routine. Most 
% time information on Macs (including Ticks, Microseconds, and Matlab's
% CPUTIME) is derived from the Mac OS Time Manager's timebase, which
% is interrupt-driven. The Time Manager loses time while interrupts are
% suppressed, whereas UpTime is unaffected.
% 
% GetSecs, WaitSecs, and several other Psychtoolbox MEX files 
% (KbCheck, KbWait, and Screen PeekBlanking) use the
% VideoToolbox Seconds.c function as their timebase. Seconds.c uses
% the Mac OS UpTime routine if available. (Otherwise it uses the Time
% Manager's Microseconds routine.) UpTime returns the value of a
% hardware counter inside the PowerPC processor, which provides
% extremely stable reliable timing. 
% 
% The Mac OS conversion from an UpTime count to a time in secs uses an
% Apple-supplied conversion factor that in Mac OS 9.0 was only accurate to
% about 1%. Apparently that's been fixed, as in Mac OS 9.2.2 the factor
% seems to be very accurate. See Apple's Technical Q&A HW31 "UpTime's
% values are consistently slow?"
% web http://developer.apple.com/qa/hw/hw31.html
% Try running TimingTest.
% If GetSecs is running slow or fast (e.g. because you're using Mac OS
% 9.0) you can scale all values returned by Seconds.c by setting the scale
% factor different from 1 in Screen Preference SecondsMultiplier. Run
% GetSecsTest to do this automatically.
% 
% THE CLOCK CHIP: The third timebase is the battery operated
% clock chip, which counts whole seconds. Normally the Mac OS
% reads this chip (by calling ReadDateTime) only at System startup and
% writes to it only if you explicitly change the time of day (eg in
% the Date & Time control panel). After startup, the time of day
% (returned by GetDateTime) is updated by the Time Manager,
% independent of the clock chip until the next restart. Changing the
% time of day, which writes to the clock chip, does not affect the
% time (since startup) returned by Microseconds and Ticks.
% 
% OS9: TESTING: As noted above, the normal way to use GetSecs is
%	s=GetSecs;
% Another way to call GetSecs is documented below, but it violates a 
% Matlab rule, and is intended primarily for testing purposes, not 
% recommended for general use. The alternative way to call GetSecs, 
% is to ask GetSecs to modify the input argument:
%	GetSecs(s)
% Or you can specify a subscript for s:
%	GetSecs(s,subscript)
% which is nearly equivalent to writing:
%	s(subscript)=GetSecs;
% The only reason to ask GetSecs to modify the input argument is to
% avoid asking the Memory Manager to allocate space, since that may
% occasionally take a long time.
%
% Win : ___________________________________________________________________
%
% On Windows machines the high precision GetPerformanceCounter() call 
% is used to get the number of seconds since system start up, if a 
% performance counter is available. Otherwise, the less accurate 
% GetCurrentTick() is used. 
% 
% See also: GetSecsTick, WaitSecs, GetTicks, WaitTicks, GetSecsTest, 
% Screen PeekBlanking.

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
